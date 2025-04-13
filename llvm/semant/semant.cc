// ASTNode 派生类的 TypeCheck() 实现
#include "../../include/ast.h"
#include "semant.h"
//#include "../include/ir.h"
#include "../include/type.h"
//#include "../include/Instruction.h"
#include<map>


//extern LLVMIR llvmIR;
extern std::map<std::pair<BuiltinType::BuiltinKind, BuiltinType::BuiltinKind>, NodeAttribute (*)(NodeAttribute, NodeAttribute, OpType::Op)> SemantBinaryNodeMap;
extern std::map<BuiltinType::BuiltinKind, NodeAttribute (*)(NodeAttribute, OpType::Op)> SemantSingleNodeMap;
extern NodeAttribute SemantError(NodeAttribute a, OpType::Op opcode);
SemantTable semant_table;
std::vector<std::string> error_msgs{}; // 将语义错误信息保存到该变量中
static int GlobalVarCnt = 0;
static int GlobalStrCnt = 0;
//std::map<std::string, NodeAttribute> ConstGlobalMap;
//std::map<std::string, NodeAttribute> StaticGlobalMap;
static int whileCount = 0;


//数组声明时，根据初始化列表收集初始化值
/*
int a[2][2][2][2]={    {  {{1, 2},{ 3, 4}},  {{ 5, 6},{ 7, 8}}   },
                       {  {{9,10},{11,12}},  {{13,14},{15,16}}   } 
                  }

int a[2][2][2][2]={    {  {{    },{3,  }},   { 5, 6 ,{7, 8}}     },
                       {  { 9,10 ,11,12 },     13,14,15,16       } 
                  }
（1）needed_num代表当前层次需要的初始值个数；
    仅在递归过程中纵向传递，不可横向共用。

（2）depth代表当前层次（由{}层数决定）：
    若当前is_exp!=0，说明又进入一层{}，否则是单值；
    第一层depth=0,needed_num[0]=total_num；依此类推;
    is_exp!=0  -->  depth++  -->  needed_num[depth] = needed_num[depth-1]/attr.dims[depth-1]

（3）dfs

 (4) IR新增逻辑：遇到显示初始化为0的值，记录其位置；

*/
void GatherArrayrecursion(NodeAttribute & attr, InitValBase init,std::vector<int> needed_num,int depth,int& pos){
    //curr_num : 这一层已经收集到的初始值个数
    int curr_num=0;
    //initval包含了该层{}内   所有显式给出的初始值
    std::vector<InitValBase> initval{};
    if(attr.ConstTag){
        initval=*(((ConstInitValList*)init)->initval);//改动1
    }else{
        initval=*(((VarInitValList*)init)->initval);//改动2
    }

    //int型
    if(attr.type->builtinKind==BuiltinType::Int){
        for(int i=0;i<initval.size();i++){
            //若不再是列表而是单值，直接加入
            if(initval[i]->getExp()!=nullptr){
                attr.IntInitVals.push_back(initval[i]->attribute.val.IntVal);
                curr_num++; 
                attr.RealInitvalPos.push_back(++pos);//全局位置
            }//若是列表，递归解析
            else{
                GatherArrayrecursion(attr,initval[i],needed_num,depth+1,pos);
                curr_num+=needed_num[depth+1];
            }
        }
        //补齐该层元素个数
        for(int i=curr_num;i<needed_num[depth];i++){
            attr.IntInitVals.push_back(0);
            ++pos;
            attr.RealInitvalPos.push_back(0);//全局位置设为0
        }
        return;
    //float型
    }else if(attr.type->builtinKind==BuiltinType::Float){
        for(int i=0;i<initval.size();i++){
            //initval[i]->TypeCheck();
            //若不再是列表而是单值，直接加入
            if(initval[i]->getExp()!=nullptr){
                attr.FloatInitVals.push_back(initval[i]->attribute.val.FloatVal);
                curr_num++;
                attr.RealInitvalPos.push_back(++pos);//全局位置
            }//若是列表，递归解析
            else{
                GatherArrayrecursion(attr,initval[i],needed_num,depth+1,pos);
            }
        }
        //补齐该层元素个数
        for(int i=curr_num;i<needed_num[depth];i++){
            attr.FloatInitVals.push_back(0.0);
            ++pos;
            attr.RealInitvalPos.push_back(0);//全局位置设为0
        }
        return;
    }
}

void GatherArrayInitVals(NodeAttribute & attr, InitValBase init){
/* 辅助信息准备:total_num , needed_num[]*/
    //计算total_num：数组中元素总数
    int total_num=attr.dims[0];//前提：dims不能为空，且正整数
    for(int i=1;i<attr.dims.size();i++){
        total_num*=attr.dims[i];
    }
    //计算needed_num[i]:各层需要的初始值个数
    std::vector<int> needed_num{};
    needed_num.resize(attr.dims.size());
    needed_num[0]=total_num;
    for(int depth=1;depth<needed_num.size();depth++){
        needed_num[depth]=needed_num[depth-1]/attr.dims[depth-1];
    }

    //初始化depth，表示当前层次 ；pos表示当前值的位置序号
    int depth=0;
    int pos=0; //整个数组的,[1,size()]
    attr.RealInitvalPos.clear();
    /* 解析初始化列表 */
    GatherArrayrecursion(attr,init,needed_num,depth,pos);
    return;
}

void InitializeSingleValue(InitValBase init, NodeAttribute &val, BuiltinType* initval_type) {
    if (init->getExp() != nullptr) {
        if (init->getExp()->attribute.type->builtinKind == BuiltinType::Void) {
            error_msgs.push_back("exp can not be void in initval in line " + std::to_string(init->line) + "\n");
        } else if (initval_type->builtinKind == BuiltinType::Int) {
            val.IntInitVals[0] = (init->getExp()->attribute.type->builtinKind == BuiltinType::Float) ?
                                  int(init->getExp()->attribute.val.FloatVal) :
                                  init->getExp()->attribute.val.IntVal;
        } else if (initval_type->builtinKind == BuiltinType::Float) {
            val.FloatInitVals[0] = (init->getExp()->attribute.type->builtinKind == BuiltinType::Float) ? 
                                    init->getExp()->attribute.val.FloatVal : 
                                    float(init->getExp()->attribute.val.IntVal);
        }
    }
}
void SolveInitVal(InitValBase init, NodeAttribute &val, BuiltinType* initval_type) {
    val.type= initval_type;
    if (val.dims.empty()) {
        InitializeSingleValue(init, val, initval_type);
    } else {
        if (init->getExp() != nullptr) {
            error_msgs.push_back("InitVal can not be exp in line " + std::to_string(init->line) + "\n");
            return;
        }
        GatherArrayInitVals(val, init);
    }
}

int GetArrayVal(NodeAttribute &val,std::vector<int> &arrayIndexes,BuiltinType::BuiltinKind type)
{
    int index=0;//获取多维数组展开后的线性索引
    for(int i=0;i<arrayIndexes.size();i++)
    {
        index *=val.dims[i];
        index +=arrayIndexes[i];
    }//val.IntInitVals,FloatInitVal中线性存放着各个数组元素
    if (type==BuiltinType::Int){return val.IntInitVals[index];}
    else if (type==BuiltinType::Float){return val.FloatInitVals[index];}//暂时只可能有参数为int或者float的情况
    
}

//可能需要完善
void CheckArrayDim(ExprBase &d,int line_number)
{
    // if(d->attribute.type->builtinKind==Type::VOID)//错误处理->出现错误仍把Intval压入，确保arrayIndexes中的个数一致
    // {
    //     error_msgs.push_back("Void is not allowed in  Array Dim in line " + std::to_string(line_number) + "\n");
    // }
    if(d->attribute.type->builtinKind==BuiltinType::Float)//错误处理
    {
        error_msgs.push_back("Float is not allowed in  Array Dim in line " + std::to_string(line_number) + "\n");
    }
    else if(d->attribute.type->builtinKind==BuiltinType::Bool)//错误处理
    {
        error_msgs.push_back("Bool is not allowed in  Array Dim in line " + std::to_string(line_number) + "\n");
    }//a[-1]表示a[]
    // else if(d->attribute.V.val.IntVal<=0)//增加对数组个数必须大于0的检查 ex,a[-1]
    // {
    //     error_msgs.push_back("Array Dim must more than zero in line " + std::to_string(line_number) + "\n");
    // }

}


void __Program::TypeCheck() {
    semant_table.symbol_table.beginScope();
    auto comp_vector = *comp_list;
    for (auto comp : comp_vector) {
        comp->TypeCheck();
    }
    for(auto [s,f] : semant_table.FunctionTable){
        if(s->getName() == "main"){
            return;
        }
    }
    //main函数的错误处理
    error_msgs.push_back("There is no main function.\n");
}


void Exp::TypeCheck() 
{
    addExp->TypeCheck();

    attribute = addExp->attribute;
}
void ConstExp::TypeCheck() 
{
    addExp->TypeCheck();
    attribute = addExp->attribute;
    if (!attribute.ConstTag) {    // addexp is not const
        error_msgs.push_back("Expression is not const " + std::to_string(addExp->line) + "\n");
    }
}
void AddExp::TypeCheck() 
{
    addExp->TypeCheck();
    mulExp->TypeCheck();
    auto key = std::make_pair(addExp->attribute.type->builtinKind, mulExp->attribute.type->builtinKind);
    auto it = SemantBinaryNodeMap.find(key);
    if (it != SemantBinaryNodeMap.end()) {
        attribute = it->second(addExp->attribute, mulExp->attribute,op.optype);
       
    } else
    {
        error_msgs.push_back("invalid operators in line " + std::to_string(addExp->attribute.line_number) + "\n");
    }
}
void MulExp::TypeCheck() 
{
    mulExp->TypeCheck();
    unaryExp->TypeCheck();
    auto key = std::make_pair(mulExp->attribute.type->builtinKind,unaryExp->attribute.type->builtinKind);
    auto it = SemantBinaryNodeMap.find(key);
    if (it != SemantBinaryNodeMap.end()) {
        attribute = it->second(mulExp->attribute, unaryExp->attribute, op.optype);
    } else
    {
         error_msgs.push_back("invalid operators in line " + std::to_string(mulExp->attribute.line_number) + "\n");
    }
}
void RelExp::TypeCheck() 
{
    relExp->TypeCheck();
    addExp->TypeCheck();
    auto key = std::make_pair(relExp->attribute.type->builtinKind,addExp->attribute.type->builtinKind);
    auto it = SemantBinaryNodeMap.find(key);
    if (it != SemantBinaryNodeMap.end()) {
        attribute = it->second(relExp->attribute, addExp->attribute, op.optype);
        
    } else
    {
         error_msgs.push_back("invalid operators in line " + std::to_string(relExp->attribute.line_number) + "\n");
    }
}
void EqExp::TypeCheck() 
{
    eqExp->TypeCheck();
    relExp->TypeCheck();
    auto key = std::make_pair(eqExp->attribute.type->builtinKind,relExp->attribute.type->builtinKind);
    auto it = SemantBinaryNodeMap.find(key);
    if (it != SemantBinaryNodeMap.end()) {
        attribute = it->second(eqExp->attribute, relExp->attribute,op.optype);
        
    } else
    {
          error_msgs.push_back("invalid operators in line " + std::to_string(eqExp->attribute.line_number) + "\n");
    }
}
void LAndExp::TypeCheck() 
{
    lAndExp->TypeCheck();
    eqExp->TypeCheck();
    auto key = std::make_pair(lAndExp->attribute.type->builtinKind,eqExp->attribute.type->builtinKind);
    auto it = SemantBinaryNodeMap.find(key);
    if (it != SemantBinaryNodeMap.end()) {
        attribute = it->second(lAndExp->attribute, eqExp->attribute, OpType::And);
    } else
    {
         error_msgs.push_back("invalid operators in line " + std::to_string(lAndExp->attribute.line_number) + "\n");
    }
}
void LOrExp::TypeCheck() 
{
    lOrExp->TypeCheck();
    lAndExp->TypeCheck();
    auto key = std::make_pair(lOrExp->attribute.type->builtinKind,lAndExp->attribute.type->builtinKind);
    auto it = SemantBinaryNodeMap.find(key);
    if (it != SemantBinaryNodeMap.end()) {
        attribute = it->second(lOrExp->attribute, lAndExp->attribute, OpType::Or);
    } else
    {
          error_msgs.push_back("invalid operators in line " + std::to_string(lOrExp->attribute.line_number) + "\n");
    }
}
void Lval::TypeCheck() 
{
    is_left=false;
    //1.查找左值name是否已经定义
    NodeAttribute val=semant_table.symbol_table.look(name); // 返回离当前作用域最近的局部变量的相关信息
    if(val.type->builtinKind!=BuiltinType::Void)//说明是局部变量，找到了val(因为如果没有在局部变量的表symbol_table中找到，会返回NodeAttribute()，默认参数type为void）
    {
        scope=semant_table.symbol_table.findScope(name);// 返回离当前作用域最近的局部变量的作用域
    }
    else if(semant_table.GlobalTable.find(name)!=semant_table.GlobalTable.end())//在全局变量表中找到了
    {
        val=semant_table.GlobalTable[name];
        scope=0;//0代表全局变量作用域
    }
    else
    {
        error_msgs.push_back("Undefined var in line " + std::to_string(line) + "\n");
        return;//未定义的变量，直接返回，不进行后续操作
    }
    //2.处理数组[]部分
    std::vector<int> arrayIndexes;
    bool arrayindexConstTag=true;//有效位
    if(dims!=nullptr)
    {
        for(auto d:*dims)
        {
            CheckArrayDim(d,line);
            d->TypeCheck();//d是arraydim
            arrayIndexes.push_back(d->attribute.val.IntVal);
            arrayindexConstTag &=d->attribute.ConstTag;//需要数组元素都有效，arrayIndexes才有效
        }
    }

    //3.判断数组使用规范
    if(arrayIndexes.size()==val.dims.size()){//使用维度与定义维度相同，比如定义a[2][3]，使用a[0][1]
        attribute.ConstTag=val.ConstTag&arrayindexConstTag;
        attribute.type=val.type;
        if(attribute.ConstTag)
        {
            if(attribute.type->builtinKind==BuiltinType::Int){attribute.val.IntVal=GetArrayVal(val,arrayIndexes,BuiltinType::Int);}
            else if(attribute.type->builtinKind==BuiltinType::Float){attribute.val.FloatVal=GetArrayVal(val,arrayIndexes,BuiltinType::Float);}
        }
    }
    else if(arrayIndexes.size()<val.dims.size())//使用维度小于定义维度，比如定义a[2][3]，使用a[0]
    {
        attribute.ConstTag=false;
        attribute.T->kind=BuiltinType::Pointer;//???
    }
    else{
        error_msgs.push_back("Array size is larger than defined size in line " + std::to_string(line) + "\n");
    }
}
void FuncRParams::TypeCheck() 
{
    for (auto param : *rParams) {
        param->TypeCheck();
        if (param->attribute.type->builtinKind == BuiltinType::Void) {
            error_msgs.push_back("FuncRParam is void in line " + std::to_string(line) + "\n");
        }
    }
}
void FuncCall::TypeCheck() 
{
     //1.查找函数是否存在
     auto it = semant_table.FunctionTable.find(name);
     if (it == semant_table.FunctionTable.end()) {
         error_msgs.push_back("Function is undefined in line " + std::to_string(line) + "\n");
         return;
     }
     FuncDef funcdef = it->second;
     attribute.type = (BuiltinType*)(semant_table.FunctionTable[name])->return_type;//需要检查类型转换是否成功
     attribute.ConstTag = false;
     
     //2.检查函数调用传入的参数
     int funcRParams_len = 0;
     if (funcRParams != nullptr) {
         funcRParams->TypeCheck();
         auto params = ((FuncRParams *)funcRParams)->rParams;
         funcRParams_len = params->size(); 
     }
     
     if ((funcdef->formals)->size() != funcRParams_len) {
         error_msgs.push_back("Function FuncFParams and FuncRParams size are not matched in line " +
                             std::to_string(line) + "\n");
     }
}

void UnaryExp::TypeCheck() 
{
    unaryExp->TypeCheck();
    attribute=SemantSingleNodeMap[unaryExp->attribute.type->builtinKind]
        (unaryExp->attribute,unaryOp.optype);
    
}
void IntConst::TypeCheck() 
{
    attribute.type = new BuiltinType(BuiltinType::Int);
    attribute.ConstTag = true;
    attribute.val.IntVal = val;
}
void FloatConst::TypeCheck() 
{
    attribute.type = new BuiltinType(BuiltinType::Float);
    attribute.ConstTag = true;
    attribute.val.FloatVal = val;
}
void PrimaryExp::TypeCheck() 
{
    exp->TypeCheck();
    attribute = exp->attribute;
}
void AssignStmt::TypeCheck() 
{
    lval->TypeCheck();//此时is_left=false
    exp->TypeCheck();
    ((Lval*)lval)->is_left=true;
    if (exp->attribute.type->builtinKind == BuiltinType::Void) {
        error_msgs.push_back("void type can not be assign_stmt's expression " + std::to_string(line) + "\n");
    }
}
void ExprStmt::TypeCheck() 
{
    exp->TypeCheck();
    attribute = exp->attribute;
}
void BlockStmt::TypeCheck() 
{
    b->TypeCheck();
}
void IfStmt::TypeCheck() 
{
    Cond->TypeCheck();
    if (Cond->attribute.type->builtinKind == BuiltinType::Void) {
        error_msgs.push_back("if cond type is invalid " + std::to_string(line) + "\n");
    }
    ifStmt->TypeCheck();
    elseStmt->TypeCheck();//???else一定存在吗
}
void WhileStmt::TypeCheck() 
{
    Cond->TypeCheck();
    if (Cond->attribute.type->builtinKind == BuiltinType::Void) {
        error_msgs.push_back("while cond type is invalid " + std::to_string(line) + "\n");
    }
    whileCount++;
    loopBody->TypeCheck();
    whileCount--;
}
void ContinueStmt::TypeCheck() 
{
    if (!whileCount) {
        error_msgs.push_back("Continue is not in while stmt in line " + std::to_string(line) + "\n");
    }
}
void BreakStmt::TypeCheck() 
{
    if (!whileCount) {
        error_msgs.push_back("Break is not in while stmt in line " + std::to_string(line) + "\n");
    }
}
void RetStmt::TypeCheck() 
{
    retExp->TypeCheck(); 
     if (retExp->attribute.type->builtinKind == BuiltinType::Void) {
        error_msgs.push_back("Return type is invalid in line " + std::to_string(line) + "\n");
    }
}
void ConstInitValList::TypeCheck() 
{
    for(auto init:*initval){init->TypeCheck();}
}
void ConstInitVal::TypeCheck() 
{
    if(exp==nullptr){return;}
    exp->TypeCheck();
    attribute =exp->attribute;
    if (!attribute.ConstTag) {    // exp is not const
        error_msgs.push_back("Expression is not const " + std::to_string(line) + "\n");
    }
    if (attribute.type->builtinKind == BuiltinType::Void) {
        error_msgs.push_back("Initval expression can not be void in line " + std::to_string(line) + "\n");
    }
}
void VarInitValList::TypeCheck() 
{
    for(auto init:*initval){init->TypeCheck();}
}
void VarInitVal::TypeCheck() 
{
    if (exp == nullptr) {
        return;
    }

    exp->TypeCheck();
    attribute = exp->attribute;

    if (attribute.type->builtinKind == BuiltinType::Void) {
        error_msgs.push_back("Initval expression can not be void in line " + std::to_string(line) + "\n");
    }
}
void VarDef_no_init::TypeCheck() 
{
    NodeAttribute val;
    val.ConstTag=false;
    val.type=attribute.type;
    if(dims!=nullptr&&!dims->empty())
    {
        for(auto d:*dims)
        {
            d->TypeCheck();
            CheckArrayDim(d,line);
            val.dims.push_back(d->attribute.val.IntVal);
        }
    }
    semant_table.symbol_table.enter(name,val);
    if (semant_table.symbol_table.scopesSize() == 0) {
		semant_table.GlobalTable[name] = val;
	}
}//do nothing
void VarDef::TypeCheck() 
{
    NodeAttribute val;
    val.ConstTag=false;
    val.type=attribute.type;
    if(dims!=nullptr&&!dims->empty())
    {
        for(auto d:*dims)
        {
            d->TypeCheck();
            CheckArrayDim(d,line);
            val.dims.push_back(d->attribute.val.IntVal);
        }
    }
    
    if(init!=nullptr)
    {
        init->TypeCheck();
        SolveInitVal(init,val,val.type);
    }
    semant_table.symbol_table.enter(name,val);
    if (semant_table.symbol_table.scopesSize() == 0) {
		semant_table.GlobalTable[name] = val;
	}
}
void ConstDef::TypeCheck() 
{
    NodeAttribute val;
    val.ConstTag=true;
    val.type=attribute.type;
    if(dims!=nullptr&&!dims->empty())
    {
        for(auto d:*dims)
        {
            d->TypeCheck();
            CheckArrayDim(d,line);
            val.dims.push_back(d->attribute.val.IntVal);
        }
    }
    
    if(init!=nullptr)
    {
        init->TypeCheck();
        SolveInitVal(init,val,val.type);
    }
    semant_table.symbol_table.enter(name,val);
    //是否需要ConstGlobalMap或StaticGlobalMap?
    if (semant_table.symbol_table.scopesSize() == 0) {
		semant_table.GlobalTable[name] = val;//已经加入global_table的还需不需要加入symbol_table?
	}
    if (semant_table.symbol_table.scopesSize() == 0) {
		semant_table.GlobalTable[name] = val;
	}
    
}
void VarDecl::TypeCheck() 
{
    for(auto def:*var_def_list)//逐一处理def
    {
        //1.获取当前作用域
        def->scope=semant_table.symbol_table.scopesSize();
        //2.多重定义的错误处理(这个变量声明的最近的作用域与当前作用域相同)//新增 SySyYtree.h GetName Getdims GetInit
        if(semant_table.symbol_table.findScope(def->name)==def->scope)
        {
            error_msgs.push_back("multiple definition for " + def->name->getName() + " in line " + std::to_string(line) + "\n");
        }
        //也许可以这么处理:都在def中处理
        //3.处理数组部分 
        //4.处理init
        def->TypeCheck();
        //5.加入符号表
     }
}
void ConstDecl::TypeCheck() 
{
    for(auto def:*var_def_list)//逐一处理def
    {
        def->scope=semant_table.symbol_table.scopesSize();
        //多次定义的错误处理//新增 SySyYtree.h GetName Getdims GetInit
        if(semant_table.symbol_table.findScope(def->name)==def->scope)
        {
            error_msgs.push_back("multiple definition for " + def->name->getName() + " in line " + std::to_string(line) + "\n");
        } 
    }
    
}
void BlockItem_Decl::TypeCheck() 
{
    decl->TypeCheck(); 
}
void BlockItem_Stmt::TypeCheck() 
{
    stmt->TypeCheck();
}
void __Block::TypeCheck() 
{
    semant_table.symbol_table.beginScope();
    auto item_vector = *item_list;
    for (auto item : item_vector) {
        item->TypeCheck();
    }
    semant_table.symbol_table.endScope();
}
void __FuncFParam::TypeCheck() 
{
    NodeAttribute val;
    val.ConstTag = false;
    val.type = (BuiltinType*)type_decl;
    scope = 1;

    // 如果dims为nullptr, 表示该变量不含数组下标, 如果你在语法分析中采用了其他方式处理，这里也需要更改
    if (dims != nullptr) {  //dims != nullptr  
        auto dim_vector = *dims;

        // the fisrt dim of FuncFParam is empty
        // eg. int f(int A[][30][40])
        val.dims.push_back(-1);    // 这里用-1表示empty，你也可以使用其他方式
        for (int i = 1; i < dim_vector.size(); ++i) {
            auto d = dim_vector[i];
            d->TypeCheck();
            CheckArrayDim(d,line);
            val.dims.push_back(d->attribute.val.IntVal);
        }
        
        attribute.T->kind =BuiltinType::Pointer;//!!!!
    } else {
        attribute.type= (BuiltinType*)(type_decl);//?
    }

    if (name != nullptr) {
        if (semant_table.symbol_table.findScope(name) != -1) {
            error_msgs.push_back("multiple difinitions of formals in function " + name->getName() + " in line " +
                                 std::to_string(line) + "\n");
        }
        semant_table.symbol_table.enter(name, val);//符号表 semant_table.symbol_table也需要函数的val
    }
}
void __FuncDef::TypeCheck() 
{
    semant_table.symbol_table.beginScope();//进入新的作用域

    semant_table.FunctionTable[name] = this;

    auto formal_vector = *formals;
    for (auto formal : formal_vector) {
        formal->TypeCheck();//逐一检查函数参数
    }

    // block TypeCheck
    if (block != nullptr) {
        auto item_vector = *(block->item_list);
        for (auto item : item_vector) {
            item->TypeCheck();
        }
    }

    semant_table.symbol_table.endScope();//退出作用域
}
void CompUnit_Decl::TypeCheck() 
{
   
    BuiltinType* type_decl=(BuiltinType*)decl->type_decl ;
    for(auto def:*decl->var_def_list)
    {
        def->scope=0;//这里获得的都是全局变量，因而作用域为0
        if(semant_table.GlobalTable.find(def->name)!=semant_table.GlobalTable.end())//说明该def的name在之前就被声明过，加入过GlbalTable了
        { error_msgs.push_back("multilpe difinitions of vars in line " + std::to_string(line) + "\n");}
        // StaticGlobalMap[def->GetName()->get_string()]=val;//?

        // BasicInstruction::LLVMType lltype = Type2LLvm[type_decl];
        // Instruction globalDecl;
        // if (def->GetDims() != nullptr) {
        //     globalDecl = new GlobalVarDefineInstruction(def->GetName()->get_string(), lltype, val);
        // } else if (init == nullptr) {
        //     globalDecl = new GlobalVarDefineInstruction(def->GetName()->get_string(), lltype, nullptr);
        // } else if (lltype == BasicInstruction::LLVMType::I32) {
        //     globalDecl =
        //     new GlobalVarDefineInstruction(def->GetName()->get_string(), lltype, new ImmI32Operand(val.IntInitVals[0]));
        // } else if (lltype == BasicInstruction::LLVMBuiltinType::Float32) {
        //     globalDecl = new GlobalVarDefineInstruction(def->GetName()->get_string(), lltype,
        //                                                 new ImmF32Operand(val.FloatInitVals[0]));
        // }
        // llvmIR.global_def.push_back(globalDecl);
    }
}
void CompUnit_FuncDef::TypeCheck() 
{
    func_def->TypeCheck();
}
