#include <bits/stdc++.h>
#include <Windows.h>
#include <conio.h>

#define fl(i,n)    for(int i = 0; i < n; i++)

#define ll   long long
#define nl   endl
#define pb   push_back
#define mp   make_pair
#define fi   first
#define se   second
#define PII  pair<int,int>
#define PSS  pair<string,string>

#define EPS  1e-9
#define INF  1e16

#define setBit(i,n)     (i|1<<n)
#define clearBit(i,n)   (i&(~(1<<n)))
#define toggleBit(i,n)  (i^1<<n)
#define isOn(i,n)       ((i&1<<n) != 0)


using namespace std;

//TODO: Revamp obj codes vector and addr into 1 vector of pairs [DONE]
//TODO: Test SIC Code [DONE]
//TODO: Edit input to include .txt automatically [DONE]
//TODO: SIC/XE Inst. set [DONE]
//TODO: Registers set [DONE]
//TODO: Handle Pass 1 (Adding +1 +2... / Symtable(Symbol, type, addr) / LTORG / Literal table (Name, value, address, size) / EQU (Expressions using stack and sym table))[DONE]
//TODO: Modify EQU to handle constants [DONE]
//TODO: Handle literals as obj code[DONE]
//TODO: Handle instructions using literals as n=i=1 or as imm or as indirect for format 3[DONE]
//TODO: Finish format 4 instruction [DONE]
//TODO: Pre-compute the M. record during format 4 obj codes [DONE]
//TODO: Words can have equations which means their obj codes will change accordingly [DONE]
//TODO: Modify HTE to take until 60 chars not just 10 T's [DONE]
//TODO: Print M.Recs [DONE]
//TODO: Handle LDA 1000 alikes [DONE]


queue<string> program;
queue<string> _program;
map<string, string> REG_SET =
{
    {"A", "0"},
    {"X", "1"},
    {"L", "2"},
    {"B", "3"},
    {"S", "4"},
    {"T", "5"},
    {"F", "6"},
};
map<string, PSS> SIC_Ins_Set =
{
    {"FIX",     {"C4", "1"}},
    {"FLOAT",   {"C0", "1"}},
    {"HIO",     {"F4", "1"}},
    {"NORM",    {"C8", "1"}},
    {"SIO",     {"F0", "1"}},
    {"TIO",     {"F8", "1"}},
    {"ADDR",    {"90", "2"}},
    {"CLEAR",   {"B4", "2"}},
    {"COMPR",   {"A0", "2"}},
    {"DIVR",    {"9C", "2"}},
    {"SHIFTL",  {"A4", "2"}},
    {"SHIFTR",  {"A8", "2"}},
    {"SUBR",    {"94", "2"}},
    {"SVC",     {"B0", "2"}},
    {"MULR",    {"98", "2"}},
    {"RMO",     {"AC", "2"}},
    {"TIXR",    {"B8", "2"}},
    {"ADD",   {"18", "3/4"}},
    {"ADDF",  {"58", "3/4"}},
    {"AND",   {"40", "3/4"}},
    {"COMP",  {"28", "3/4"}},
    {"COMPF", {"88", "3/4"}},
    {"DIV",   {"24", "3/4"}},
    {"DIVF",  {"64", "3/4"}},
    {"J",     {"3C", "3/4"}},
    {"JEQ",   {"30", "3/4"}},
    {"JGT",   {"34", "3/4"}},
    {"JLT",   {"38", "3/4"}},
    {"JSUB",  {"48", "3/4"}},
    {"LDA",   {"00", "3/4"}},
    {"LDB",   {"68", "3/4"}},
    {"LDCH",  {"50", "3/4"}},
    {"LDF",   {"70", "3/4"}},
    {"LDL",   {"08", "3/4"}},
    {"LDS",   {"6C", "3/4"}},
    {"LDT",   {"74", "3/4"}},
    {"LDX",   {"04", "3/4"}},
    {"LPS",   {"D0", "3/4"}},
    {"MUL",   {"20", "3/4"}},
    {"MULF",  {"60", "3/4"}},
    {"OR",    {"44", "3/4"}},
    {"RD",    {"D8", "3/4"}},
    {"RSUB",  {"4C", "3/4"}},
    {"TD",    {"E0", "3/4"}},
    {"TIX",   {"2C", "3/4"}},
    {"WD",    {"DC", "3/4"}},
    {"SSK",   {"EC", "3/4"}},
    {"STA",   {"0C", "3/4"}},
    {"STB",   {"78", "3/4"}},
    {"STCH",  {"54", "3/4"}},
    {"STF",   {"80", "3/4"}},
    {"STI",   {"D4", "3/4"}},
    {"STL",   {"14", "3/4"}},
    {"STS",   {"7C", "3/4"}},
    {"STSW",  {"E8", "3/4"}},
    {"STT",   {"84", "3/4"}},
    {"STX",   {"10", "3/4"}},
    {"SUB",   {"1C", "3/4"}},
    {"SUBF",  {"5C", "3/4"}},

};
vector<PSS> obj_codes;
vector<PSS> MRec;
map<string, PSS> symbol_table;
map<string, pair<PSS, string>> literal_table;
string pc, pname, paddr, pend, base;


string decimal_to_binary(int num){
    static string ret = "";
    if(num == 0){
        string res = ret;
        ret = "";
        return (reverse(res.begin(), res.end()), res += "");
    }
    ret += ((num % 2) + '0');
    return decimal_to_binary(num / 2);
}

int binary_to_decimal(string bin, int len, int i = 0){
    if(i == len - 1) return bin[i] - '0';

    return (((bin[i] - '0') << (len - i - 1)) + binary_to_decimal(bin, len, i + 1));
}

int hex_to_decimal(string hex, int len, int i = 0){
    if(i == len - 1){
        if(hex[i] >= '0' && hex[i] <= '9'){
            return (hex[i] - '0');
        }
        else if(hex[i] >= 'A' && hex[i] <= 'F'){
            return (hex[i] - '7');
        }
    }

    return ((hex[i] >= '0' && hex[i] <= '9') ? (hex[i] - '0') * pow(16, (len - i - 1)) : (hex[i] - '7') * pow(16, (len - i - 1))) + hex_to_decimal(hex, len, i + 1);
}

string decimal_to_hex(u_int num){
    static string res = "";
    if(num == 0) {
        string ret = res;
        res = "";
        return (reverse(ret.begin(), ret.end()), ret += "");
    }

    if((num % 16) >= 10) res += ((num % 16) + '7');
    else res += ((num % 16) + '0');

    return decimal_to_hex(num / 16);
}

string hex_to_binary(string hex){
    ll dec = hex_to_decimal(hex, hex.length());
    return decimal_to_binary(dec);
}

string binary_to_hex(string _bin){
    int dec = binary_to_decimal(_bin, _bin.length());
    return decimal_to_hex(dec);
}

void setTextColor(string bgcolor, string txtcolor){
    map<string, int> Colors = {
        {"Black",   0},
        {"Blue",    1},
        {"Green",   2},
        {"Aqua",    3},
        {"Red",     4},
        {"Purple",  5},
        {"Yellow",  6},
        {"White",   7},
        {"Gray",    8},
        {"LBlue",   9},
        {"LGreen",  10},
        {"LAqua",   11},
        {"LRed",    12},
        {"LPurple", 13},
        {"LYellow", 14},
        {"LWhite",  15},
    };
    HANDLE  hConsole;
	int k = Colors[txtcolor] + Colors[bgcolor]*16;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, k);
}

int countComma(string word){
    int cmms = 0;
    for(int i = 0; i < word.length(); i++){
        if(word[i] == ',') cmms++;
    }
    return cmms;
}

vector<string> splitCommas(string line){
    vector<string> ret;
    stringstream ss(line);
    string word;
    while(getline(ss, word, ',')) ret.pb(word);

    return ret;
}

string addZeros(string word, int digits){
    int zrs = digits - word.length();
    string ret = "";
    for(int i = 0; i < zrs; i++){
        ret += '0';
    }
    ret += word;
    return ret;
}

int formatSize(string word){
    string format = SIC_Ins_Set[word].se;
    int _size;
    if(format == "1") _size = 8;
    else if(format == "2") _size = 16;
    else {
        if(word.find("+") != string::npos){
            _size = 32;
        }
        else {
            _size = 24;
        }
    }
    return (_size / 8);
}


string getObjCode(string a, string b, int X){
    string objcode = "";
    string format = SIC_Ins_Set[a].se;
    int _size;
    string opcode = SIC_Ins_Set[a].fi;
    if(format == "1"){
        _size = 8;
        objcode = addZeros(opcode, _size);
    }
    else if(format == "2"){
        _size = 16;
        vector<string> regs = splitCommas(b);
        if(regs.size() > 1) objcode += opcode, objcode += REG_SET[regs[0]], objcode += REG_SET[regs[1]];
        else objcode += opcode, objcode += REG_SET[regs[0]], objcode += '0';
    }
    else {
        if(a.find("+") != string::npos){
            _size = 32;
            a.erase(a.begin());
            opcode = SIC_Ins_Set[a].fi;
            objcode += hex_to_binary(opcode);
            objcode = addZeros(objcode, 8);
            objcode.erase(objcode.length() - 1), objcode.erase(objcode.length() - 1);
            int n = 1, i = 1;
            int symaddr;
            bool sym = 1;
            if(b.find("#") != string::npos){
                b.erase(b.begin());
                n = 0, i = 1;
                if(isdigit(b[0])){
                    sym = 0;
                    stringstream ss(b);
                    ss >> symaddr;
                }
            }
            else if(b.find("@") != string::npos){
                b.erase(b.begin());
                n = 1, i = 0;
            }
            objcode += to_string(n), objcode += to_string(i), objcode += to_string(X);
            objcode += "001";
            if(sym){
                if(symbol_table.find(b) != symbol_table.end()){
                    symaddr = hex_to_decimal(symbol_table[b].se, symbol_table[b].se.length());
                }
                else {
                    symaddr = hex_to_decimal(literal_table[b].se, literal_table[b].se.length());
                }
            }
            objcode = binary_to_hex(objcode);
            objcode += addZeros(decimal_to_hex(symaddr), 5);
            objcode = addZeros(objcode, 8);
            MRec.pb(mp(addZeros(decimal_to_hex(hex_to_decimal(pc, pc.length()) + 1), 6), "05"));

        }
        else {
            _size = 24;
            objcode += hex_to_binary(opcode);
            objcode = addZeros(objcode, 8);
            objcode.erase(objcode.length() - 1), objcode.erase(objcode.length() - 1);
            int n = 1, i = 1;
            int symaddr;
            bool sym = 1, modif = 1;
            for(int i = 0; i < b.length(); i++){
                if(!isdigit(b[i])){
                    modif = 0;
                    break;
                }
            }
            if(modif){
                objcode += to_string(n), objcode += to_string(i), objcode += to_string(X), objcode += "000";
                stringstream ss(b);
                ss >> symaddr;
                objcode = binary_to_hex(objcode);
                objcode += addZeros(decimal_to_hex(symaddr), 3);
                objcode = addZeros(objcode, 6);
                MRec.pb(mp(addZeros(decimal_to_hex(hex_to_decimal(pc, pc.length()) + 1), 6), "03"));
                return objcode;
            }

            if(b.find("#") != string::npos){
                b.erase(b.begin());
                n = 0, i = 1;
                if(isdigit(b[0])){
                    sym = 0;
                    stringstream ss(b);
                    ss >> symaddr;
                }
            }
            else if(b.find("@") != string::npos){
                b.erase(b.begin());
                n = 1, i = 0;
            }
            objcode += to_string(n), objcode += to_string(i), objcode += to_string(X);
            int _pc = hex_to_decimal(pc, pc.length()) + (_size / 8);
            if(sym){
                if(symbol_table.find(b) != symbol_table.end()){
                    symaddr = hex_to_decimal(symbol_table[b].se, symbol_table[b].se.length());
                }
                else {
                    symaddr = hex_to_decimal(literal_table[b].se, literal_table[b].se.length());
                }
            }

            int disp;
            int b = 0, p = 0;
            string _toHex;
            if(sym){
                disp = symaddr - _pc;

                if(disp < 0){
                    u_int toHex = disp;
                    _toHex = decimal_to_hex(toHex);
                }
                else {
                    _toHex = addZeros(decimal_to_hex(disp), 3);
                }

                int c = 0;
                bool fs = 0;
                for(int i = 0; i < _toHex.length(); i++){
                    if(_toHex[i] != 'F' && _toHex[i] != '0') fs = 1;
                    if(fs) c++;
                }

                if(c <= 3){
                    p = 1;
                    _toHex = _toHex.substr(_toHex.length() - 3);
                }
                else {
                    b = 1;
                    disp = symaddr - hex_to_decimal(base, base.length());
                    if(disp < 0){
                        u_int toHex = disp;
                        _toHex = decimal_to_hex(toHex);
                    }
                    else {
                        _toHex = addZeros(decimal_to_hex(disp), 3);
                    }
                }
            }
            else _toHex = decimal_to_hex(symaddr);
            objcode += to_string(b), objcode += to_string(p), objcode += "0";
            objcode = binary_to_hex(objcode);
            objcode += addZeros(_toHex, 3);
            objcode = addZeros(objcode, 6);
        }
    }

    return objcode;
}

string content_of(string word){
    string ret = "";
    bool flag = false;
    for(int i = 0; i < word.length(); i++){
        if(word[i] == '\''){
            if(flag) break;
            else {
                flag = true;
                continue;
            }
        }
        if(flag) ret += word[i];
    }

    return ret;
}

int findBYTESize(string word){
    string res = content_of(word);
    if(word[0] == '=') return (word[1] == 'X' ? ceil(res.length() / 2.0) : res.length());
    return (word[0] == 'X' ? ceil(res.length() / 2.0) : res.length());
}

string getCharObj(char c){
    int dec = c;
    return addZeros(decimal_to_hex(dec), 2);
}

bool isOperator(char a){
    return (a == '+' || a == '-' || a == '*');
}

int solveOperation(int a, int b, char op){
    if(op == '+'){
        return a + b;
    }
    else if(op == '-'){
        return a - b;
    }
    else return a * b;

}

int preced(char op){
    if(op == '+' || op == '-') return 1;
    else if (op == '*') return 2;
    else return 0;
}

int solveEQN(string eqn){
    stack<char> checker;
    bool flag = 1;
    for(int i = 0; i < eqn.length(); i++){
        if(eqn[i] == '(' || eqn[i] == '[' || eqn[i] == '{')
            flag = 0, checker.push(eqn[i]);
        if(eqn[i] == ')'){
            if(checker.top() == '(') checker.pop();
        }
        else if(eqn[i] == ']'){
            if(checker.top() == '[') checker.pop();
        }
        else if(eqn[i] == '}'){
            if(checker.top() == '{') checker.pop();
        }
    }
    if(flag || checker.empty()){
        stack<int> nums;
        stack<char> ops;
        string word = "";
        for(int i = 0; i < eqn.length(); i++){
            if(isdigit(eqn[i])){
                int num = 0;
                while(isdigit(eqn[i])){
                    num = (num * 10) + (eqn[i] - '0');
                    i++;
                }
                nums.push(num);
                i--;
            }
            else if(eqn[i] == '('){
               ops.push(eqn[i]);
               if(word != ""){
                   int val = hex_to_decimal(symbol_table[word].se, (symbol_table[word].se).length());
                   nums.push(val);
               }
               word = "";
            }
            else if(eqn[i] == ')'){
               if(word != ""){
                   int val = hex_to_decimal(symbol_table[word].se, (symbol_table[word].se).length());
                   nums.push(val);
               }
               while(!ops.empty() && ops.top() != '('){
                     int num1 = nums.top();
                     nums.pop();

                     int num2 = nums.top();
                     nums.pop();

                     char op = ops.top();
                     ops.pop();

                     nums.push(solveOperation(num2, num1, op));
               }
               ops.pop();
               word = "";
            }
            else if(isOperator(eqn[i])){
               if(word != ""){
                   int val = hex_to_decimal(symbol_table[word].se, (symbol_table[word].se).length());
                   nums.push(val);
               }

               while(!ops.empty() && (preced(ops.top()) >= preced(eqn[i]))){
                     int num1 = nums.top();
                     nums.pop();

                     int num2 = nums.top();
                     nums.pop();

                     char op = ops.top();
                     ops.pop();

                     nums.push(solveOperation(num2, num1, op));
               }
               ops.push(eqn[i]);
               word = "";
            }
            else if(eqn[i] != '(' && eqn[i] != ')' && !isOperator(eqn[i])){
                word += eqn[i];
            }

        }
        if(word != "") nums.push(hex_to_decimal(symbol_table[word].se, (symbol_table[word].se).length()));

        while(!ops.empty()){
            int num1 = nums.top();
            nums.pop();

            int num2 = nums.top();
            nums.pop();

            char op = ops.top();
            ops.pop();

            nums.push(solveOperation(num2, num1, op));
        }
        return nums.top();
    }
    else {
        setTextColor("Black", "LRed");
        system("CLS");
        cout << "ERROR: Formatting error, try again." << nl;
        Sleep(99999999);

    }
    return -1;
}

bool visited[10000+1];
int findIndex(string s){
    for(int i = 0; i < obj_codes.size(); i++){
        if(obj_codes[i].fi == s && !visited[i]){
            visited[i] = true;
            return i;
        }
    }
    return -1;
}



vector<string> _literals;
void Pass1_handleLine(string line){
    stringstream ss(line);
    vector<string> words;
    while(ss >> line) words.pb(line);
    int sz = words.size();
    string type;
    vector<string>::iterator it;
    bool f2 = 0;
    for(it = words.begin(); it != words.end(); it++){
        if(SIC_Ins_Set.find((*it)) != SIC_Ins_Set.end()){
            int _t = formatSize((*it));
            if(_t == 2) f2 = 1;
        }
        if((*it)[(*it).length() - 1] == 'X' && (*it)[(*it).length() - 2] == ',' && !f2){
            setTextColor("Black", "LRed");
            cout << "ERROR: Formatting error, make sure all indentation is correct and try again" << nl;
            Sleep(99999999);
            return;
        }
        if((*it)[(*it).length() - 1] == ','){
            (*it).erase((*it).length() - 1);
            sz--;
        }
        if((*it).find("=") != string::npos){
            _literals.pb((*it));
        }
    }

    int _pc = hex_to_decimal(pc, pc.length());
    if(sz >= 3){
        if(words[2].find("-") != string::npos){
            vector<string> minuses;
            vector<string> pluses;
            stringstream ss(words[2]);
            string word;
            while(getline(ss, word, '-')) minuses.pb(word);
            stringstream s(words[2]);
            while(getline(s, word, '+')) pluses.pb(word);

            if((pluses.size()) != (minuses.size() - 1)) type = "R";
            else type = "A";
        }
        else type = "R";
        if(words[1] != "EQU") symbol_table.insert(mp(words[0], mp(type, pc)));
        if(words[1] == "RESW"){
            stringstream ss(words[2]);
            int len;
            ss >> len;
            _pc += (len * 3);
        }
        else if(words[1] == "RESB"){
            stringstream ss(words[2]);
            int len;
            ss >> len;
            _pc += len;
        }
        else if(words[1] == "WORD"){
            int len = countComma(words[2]) + 1;
            len *= 3;
            _pc += len;
        }
        else if(words[1] == "BYTE"){
            int len = findBYTESize(words[2]);
            if(len > 1 && content_of(words[2]).length() % 2 == 1 && words[2][0] != 'C'){
                setTextColor("Black", "LRed");
                cerr << "ERROR: BYTE arrays size must be multiples of 2." << nl;
                Sleep(1000);
                cerr << "SYSTEM: Parsing stopped...";
                Sleep(9999999);
                return;
            }
            _pc += len;
        }
        else if(words[1] == "BASE" || words[1] == "LTORG" || words[1] == "EQU"){
            if(words[1] == "EQU"){
                string eqn = words[2];
                if(eqn == "*") symbol_table.insert(mp(words[0], mp(type, pc)));
                else if(isdigit(eqn[0])){
                    bool flag = 1;
                    int num = 0;
                    for(int i = 0; i < eqn.length(); i++){
                        if(!isdigit(eqn[i])) {
                            flag = 0;
                            break;
                        }
                        else {
                            num = (num * 10) + (eqn[i] - '0');
                        }
                    }
                    if(!flag){
                        int locaddr = solveEQN(words[2]);
                        symbol_table.insert(mp(words[0], mp(type, decimal_to_hex(locaddr))));
                    }
                    else {
                        symbol_table.insert(mp(words[0], mp(type, addZeros(to_string(num), 4))));
                    }
                }
                else {
                    int locaddr = solveEQN(words[2]);
                    symbol_table.insert(mp(words[0], mp(type, decimal_to_hex(locaddr))));
                }
            }
        }
        else {
            _pc += formatSize(words[1]);
        }
    }
    else {
        if(words[0] == "BASE" || words[0] == "LTORG" || words[0] == "EQU" || words[0] == "End"){
            if(words[0] == "LTORG" || words[0] == "End"){
                for(int i = 0; i < _literals.size(); i++){
                    int _size = findBYTESize(_literals[i]);
                    literal_table.insert(mp(_literals[i], mp(mp(content_of(_literals[i]), to_string(_size)), decimal_to_hex(_pc))));
                    _pc += _size;
                }
                if(words[0] == "End") pend = decimal_to_hex(_pc);
                _literals.clear();
            }
            else if(words[0] == "BASE"){
                if(words[1] == "*") base = pc;
            }
        }
        else {
            _pc += formatSize(words[0]);
        }

    }
    pc = decimal_to_hex(_pc);

}

void incPC(vector<string> words){
    int sz = words.size();

    vector<string>::iterator it;
    for(it = words.begin(); it != words.end(); it++){
        if((*it)[(*it).length() - 1] == ','){
            sz--;
        }
        if((*it).find("=") != string::npos){
            _literals.pb((*it));
        }
    }

    int _pc = hex_to_decimal(pc, pc.length());
    if(sz >= 3){
        if(words[1] == "RESW"){
            stringstream ss(words[2]);
            int len;
            ss >> len;
            _pc += (len * 3);
        }
        else if(words[1] == "RESB"){
            stringstream ss(words[2]);
            int len;
            ss >> len;
            _pc += len;
        }
        else if(words[1] == "WORD"){
            int len = countComma(words[2]) + 1;
            len *= 3;
            _pc += len;
        }
        else if(words[1] == "BYTE"){
            int len = findBYTESize(words[2]);
            if(len > 1 && content_of(words[2]).length() % 2 == 1 && words[2][0] != 'C'){
                setTextColor("Black", "LRed");
                cerr << "ERROR: BYTE arrays size must be multiples of 2." << nl;
                Sleep(1000);
                cerr << "SYSTEM: Parsing stopped...";
                Sleep(9999999);
                return;
            }
            _pc += len;
        }
        else if(words[1] == "BASE" || words[1] == "LTORG" || words[1] == "EQU"){

        }
        else {
            _pc += formatSize(words[1]);
        }
    }
    else {
        if(words[0] == "BASE" || words[0] == "LTORG" || words[0] == "EQU" || words[0] == "End"){
            if(words[0] == "LTORG" || words[0] == "End"){
                for(int i = 0; i < _literals.size(); i++){
                    int _size = findBYTESize(_literals[i]);
                    _pc += _size;
                }
                _literals.clear();
            }
        }
        else {
            _pc += formatSize(words[0]);
        }

    }
    pc = decimal_to_hex(_pc);
}

void Pass2_handleLine(string line){
    string _line = line;
    stringstream ss(line);
    vector<string> words;
    while(ss >> line) words.pb(line);
    if(words.size() > 1) {
        if(words[1] == "EQU"){
            obj_codes.pb(mp("GGGGGG", pc));
            return;
        }
    }
    if(words[0] == "LTORG"){
        obj_codes.pb(mp("HHHHHHH", pc));
        return;

    }
    if(words[0] == "BASE"){
        base = symbol_table[words[1]].se;
        obj_codes.pb(mp("HHHHHHH", pc));
        return;
    }
    for(auto x: literal_table){
        if((x.se).se == pc){
            if(findBYTESize(x.fi) > 1){
                string byte_array = (x.se).fi.fi;
                bool X = ((x.fi)[1] == 'X' ? true : false);
                obj_codes.pb(mp("HHHHHHH", pc));
                obj_codes.pb(mp("HHHHHH", pc));
                int _pc = hex_to_decimal(pc, pc.length());
                if(X) {
                    for(int i = 0; i < byte_array.length(); i += 2){
                        string temp = "";
                        temp += byte_array[i];
                        temp += byte_array[i + 1];
                        obj_codes.pb(mp(temp, decimal_to_hex(_pc)));
                        _pc += 1;
                    }
                }
                else {
                    for(int i = 0; i < byte_array.length(); i++){
                        string temp = getCharObj(byte_array[i]);
                        obj_codes.pb(mp(temp, decimal_to_hex(_pc)));
                        _pc += 1;
                    }
                }
                obj_codes.pb(mp("HHHHHH", decimal_to_hex(_pc)));
                obj_codes.pb(mp("HHHHHHH", decimal_to_hex(_pc)));
                pc = decimal_to_hex(_pc);
            }
            else {
                string byte = (x.se).fi.fi;
                if((x.fi)[1] == 'X') obj_codes.pb(mp(byte, pc));
                else obj_codes.pb(mp(getCharObj(byte[0]), pc));
                pc = decimal_to_hex(hex_to_decimal(pc, pc.length()) + 1);
            }
            Pass2_handleLine(_line);
            return;
        }
    }
    vector<string>::iterator it;
    bool f2 = 0;
    for(it = words.begin(); it != words.end(); it++){
        if(SIC_Ins_Set.find((*it)) != SIC_Ins_Set.end()){
            int _t = formatSize((*it));
            if(_t == 2) f2 = 1;
        }
        if((*it)[(*it).length() - 1] == 'X' && (*it)[(*it).length() - 2] == ',' && !f2){
            setTextColor("Black", "LRed");
            cout << "ERROR: Formatting error, make sure all indentation is correct and try again" << nl;
            Sleep(99999999);
            return;
        }
        if((*it)[(*it).length() - 1] == ','){
            (*it).erase((*it).length() - 1);
        }
    }



    if(words.size() >= 3){
        if(words[1] == "RESW" || words[1] == "RESB"){
            obj_codes.pb(mp("GGGGGG", pc));
        }
        else if(words[1] == "WORD"){
            if(countComma(words[2]) > 0){
                obj_codes.pb(mp("HHHHHHH", pc));
                obj_codes.pb(mp("HHHHHH", pc));
                vector<string> temp = splitCommas(words[2]);
                int _pc = hex_to_decimal(pc, pc.length());
                for(int i = 0; i < temp.size(); i++){
                    bool flag = 1;
                    int num = 0;
                    for(int j = 0; j < temp[i].length(); j++){
                        if(!isdigit(temp[i][j])) {
                            flag = 0;
                            break;
                        }
                        else {
                            num = (num * 10) + (temp[i][j] - '0');
                        }
                    }
                    if(flag){
                        string hex = decimal_to_hex(num);
                        hex = addZeros(hex, 6);
                        obj_codes.pb(mp(hex, decimal_to_hex(_pc)));
                    }
                    else {
                        if(words[2].find("+") != string::npos || words[2].find("-") != string::npos){
                            int objcode = solveEQN(temp[i]);
                            string hex = decimal_to_hex(objcode);
                            hex = addZeros(hex, 6);
                            obj_codes.pb(mp(hex, pc));
                        }
                        else {
                            string hex = addZeros(symbol_table[temp[i]].se, 6);
                            obj_codes.pb(mp(hex, pc));
                        }
                    }
                    _pc += 3;
                }
                obj_codes.pb(mp("HHHHHH", decimal_to_hex(_pc)));
                obj_codes.pb(mp("HHHHHHH", decimal_to_hex(_pc)));
            }
            else {
                bool flag = 1;
                int num = 0;
                for(int i = 0; i < words[2].length(); i++){
                    if(!isdigit(words[2][i])) {
                        flag = 0;
                        break;
                    }
                    else {
                        num = (num * 10) + (words[2][i] - '0');
                    }
                }
                if(flag){
                    string hex = decimal_to_hex(num);
                    hex = addZeros(hex, 6);
                    obj_codes.pb(mp(hex, pc));
                }
                else {
                    if(words[2].find("+") != string::npos || words[2].find("-") != string::npos){
                        int objcode = solveEQN(words[2]);
                        string hex = decimal_to_hex(objcode);
                        hex = addZeros(hex, 6);
                        obj_codes.pb(mp(hex, pc));
                    }
                    else {
                        string hex = addZeros(symbol_table[words[2]].se, 6);
                        obj_codes.pb(mp(hex, pc));
                    }
                }
            }
        }
        else if(words[1] == "BYTE"){
            if(findBYTESize(words[2]) > 1){
                string byte_array = content_of(words[2]);
                bool X = (words[2][0] == 'X' ? true : false);
                obj_codes.pb(mp("HHHHHHH", pc));
                obj_codes.pb(mp("HHHHHH", pc));
                int _pc = hex_to_decimal(pc, pc.length());
                if(X) {
                    for(int i = 0; i < byte_array.length(); i += 2){
                        string temp = "";
                        temp += byte_array[i];
                        temp += byte_array[i + 1];
                        obj_codes.pb(mp(temp, decimal_to_hex(_pc)));
                        _pc += 1;
                    }
                }
                else {
                    for(int i = 0; i < byte_array.length(); i++){
                        string temp = getCharObj(byte_array[i]);
                        obj_codes.pb(mp(temp, decimal_to_hex(_pc)));
                        _pc += 1;
                    }
                }
                obj_codes.pb(mp("HHHHHH", decimal_to_hex(_pc)));
                obj_codes.pb(mp("HHHHHHH", decimal_to_hex(_pc)));
            }
            else {
                string byte = content_of(words[2]);
                if(words[2][0] == 'X') obj_codes.pb(mp(byte, pc));
                else obj_codes.pb(mp(getCharObj(byte[0]), pc));
            }
        }
        else if(f2){
            vector<string> _words;
            stringstream s(line);
            while(s >> line) _words.pb(line);

            string objcode = getObjCode(words[1], words[2], 0);
            obj_codes.pb(mp(objcode, pc));
        }
        else if(words[1] == "RSUB" || words[1] == "+RSUB"){
            string objcode = "4F";
            if(words[1] == "RSUB") objcode += "0000";
            else objcode += "000000";
            obj_codes.pb(mp(objcode, pc));
        }
        else if(words[words.size() - 1] == "X" && !f2){
            string objcode;
            if(words.size() == 3){
                objcode = getObjCode(words[0], words[1], 1);
            }
            else {
                objcode = getObjCode(words[1], words[2], 1);
            }
            obj_codes.pb(mp(objcode, pc));

        }
        else {
            string objcode = getObjCode(words[1], words[2], 0);
            obj_codes.pb(mp(objcode, pc));
        }
    }
    else if(words[0] == "End"){
        pend = pc;
    }
    else if(f2){
            vector<string> _words;
            stringstream s(line);
            while(s >> line) _words.pb(line);

            string objcode = getObjCode(words[0], words[1], 0);
            obj_codes.pb(mp(objcode, pc));
    }
    else if(words[0] == "RSUB" || words[0] == "+RSUB"){
            string objcode = "4F";
            if(words[0] == "RSUB") objcode += "0000";
            else objcode += "000000";
            obj_codes.pb(mp(objcode, pc));
    }
    else {
        string objcode = getObjCode(words[0], words[1], 0);
        obj_codes.pb(mp(objcode, pc));
    }
    incPC(words);

}


void solve(){
    setTextColor("Black", "LBlue");
    cout << "Starting Pass 1..." << nl;
    Sleep(800);
    system("CLS");
    //Start of Pass 1
    string line;
    stringstream ss(program.front());
    ss >> line;
    pname = line;
    while(ss >> line);
    pc = line;
    paddr = pc;
    program.pop();

    while(!program.empty()){
        line = program.front();
        Pass1_handleLine(line);
        program.pop();
    }
    setTextColor("Black", "LBlue");
    cout << "Creating Symbol Table..." << nl;
    Sleep(1000);

    cout << nl;
    //map<string, string>::iterator it;
    int longest = 0;
    for(auto it : symbol_table){
        int len = (it.fi).length();
        longest = max(longest, len);
    }
    setTextColor("Black", "Yellow");
    for(auto it : symbol_table){
        cout << it.first;
        int spcs = longest - (it.fi).length() + 1;
        for(int i = 0; i < spcs; i++) cout << " ";
        int zeros = 4 - ((it.se).se).length();
        for(int i = 0; i < zeros; i++) cout << '0';
        cout << (it.se).se << "  " << (it.se).fi << nl;
        Sleep(50);
    }
    setTextColor("Black", "LBlue");
    cout << nl << "Creating Literal Table..." << nl;
    Sleep(1000);
    cout << nl;
    longest = 0;
    int longest_2 = 0;
    for(auto it : literal_table){
        int len = (it.fi).length();
        longest = max(longest, len);
        int len2 = ((it.se).fi).fi.length();
        longest_2 = max(longest_2, len2);
    }
    setTextColor("Black", "Yellow");
    for(auto it : literal_table){
        cout << it.fi;
        int spcs = longest - (it.fi).length() + 1;
        for(int i = 0; i < spcs; i++) cout << " ";
        cout << ((it.se).fi).fi;
        spcs = longest_2 - ((it.se).fi).fi.length() + 1;
        for(int i = 0; i < spcs; i++) cout << " ";
        cout << addZeros(((it.se).fi).se, 3) << "  " << addZeros((it.se).se, 4) << nl;
        Sleep(50);
    }


    program = _program;
    setTextColor("Black", "LGreen");
    cout << nl << "Pass 1 Successfully completed." << nl;
    Sleep(400);
    setTextColor("Black", "LPurple");
    cout << "Press any key to begin Pass 2..." << nl;
    _getch();
    system("CLS");

    setTextColor("Black", "LBlue");
    cout << "Starting Pass 2..." << nl;
    Sleep(800);
    system("CLS");

    //Start of Pass 2
    stringstream s(program.front());
    s >> line;
    pname = line;
    while(s >> line);
    pc = line;
    program.pop();
    Sleep(500);
    setTextColor("Black", "LBlue");
    cout << "Creating HTE Record File..." << nl << nl;
    Sleep(1500);

    while(!program.empty()){
        line = program.front();
        Pass2_handleLine(line);
        program.pop();
    }
    ofstream output("Output.txt");
    if(output.is_open()){
        string t = "";
        int zrs = 6 - pname.length();
        for(int i = 0; i < zrs; i++){
            t += 'X';
        }
        pname += t;

        paddr = addZeros(paddr, 6);
        int length = hex_to_decimal(pend, pend.length()) - hex_to_decimal(paddr, paddr.length());
        output << "H^" << pname << "^" << paddr << "^" << addZeros(decimal_to_hex(length), 6) << nl;

        vector<vector<string>> T;
        vector<string> v;
        bool working = false;
        for(int i = 0, c = 0; i < obj_codes.size(); i++){
            if(obj_codes[i].fi == "HHHHHHH") continue;
            if((c >= 60 || obj_codes[i].fi == "GGGGGG") && obj_codes[i].fi != "HHHHHH"){
                if(c > 60){
                    c = 0;
                    string _extra = v[v.size() - 1];
                    v.erase(v.end());
                    if(v.size() > 0) T.pb(v);
                    v.clear();
                    if(obj_codes[i].fi != "GGGGGG"){
                        v.pb(_extra);
                        c += _extra.length();
                        v.pb(obj_codes[i].fi);
                        c += obj_codes[i].fi.length();
                    }
                    else {
                        v.pb(_extra);
                        T.pb(v);
                        v.clear();
                    }
                }
                else {
                    c = 0;
                    if(v.size() > 0) T.pb(v);
                    v.clear();
                    if(obj_codes[i].fi != "GGGGGG") v.pb(obj_codes[i].fi), c += obj_codes[i].fi.length();
                }
                continue;
            }
            else if(obj_codes[i].fi == "HHHHHH"){
                if(working){
                    working = false;
                    c = 0;
                    T.pb(v);
                    v.clear();
                    continue;
                }
                else {
                    working = true;
                    if(v.size() > 0) T.pb(v);
                    v.clear();
                    c = 0;
                    continue;
                }
            }
            else if(i+1 > obj_codes.size() - 1){
                if(obj_codes[i].fi != "GGGGGG") v.pb(obj_codes[i].fi);
                if(v.size() > 0) T.pb(v);
                v.clear();
                continue;
            }
            else {
                v.pb(obj_codes[i].fi);
            }

            c += obj_codes[i].fi.length();
        }

        for(int i = 0; i < T.size(); i++){
            vector<string> row = T[i];
            //for(auto x: row) cout << x << nl;
            string s = row[0];
            if(s == "NULL") continue;
            int ind = findIndex(s);
            bool flag = false;
            if((ind - 1) >= 0 && obj_codes[ind - 1].fi == "HHHHHH") flag = true;

            int start = hex_to_decimal(obj_codes[ind].se, obj_codes[ind].se.length());
            string ans1 = decimal_to_hex(start);
            ans1 = addZeros(ans1, 6);
            output << "T^" << (ans1 == ""? "000000" : ans1) << "^";
            int prevInd = ind;

            s = row[row.size() - 1];
            ind = findIndex(s);
            if(ind == -1) ind = prevInd; //Only special case with 1 T-record.
            int _end;
            if((ind + 1) < obj_codes.size()) _end = hex_to_decimal(obj_codes[ind + 1].se, obj_codes[ind + 1].se.length());
            else _end = hex_to_decimal(pend, pend.length());
            if((ind + 1) < obj_codes.size() && obj_codes[ind + 1].fi == "HHHHHH") flag = true;
            int length = _end - start;

            output << addZeros(decimal_to_hex(length), 2) << "^";

            for(int j = 0; j < T[i].size(); j++){
                output << T[i][j];
                if(j < T[i].size() - 1 && !flag) output << "^";
            }

            output << nl;
        }
        for(int i = 0; i < MRec.size(); i++){
            output << "M^" << MRec[i].fi << "^" << MRec[i].se << nl;
        }

        output << "E^" << paddr << nl;
    }
    else {
        setTextColor("Black", "LRed");
        cerr << "ERROR: Error writing to file, try again." << nl;
        Sleep(1000);
        cerr << "SYSTEM: Parsing stopped...";
        Sleep(9999999);
        return;
    }

    setTextColor("Black", "LGreen");
    cout << "Successfully parsed input file." << nl;
    Sleep(500);
    setTextColor("Black", "LYellow");
    cout << "File Output.txt was created in application root folder." << nl;
    output.close();
}


int main()
{
    char fname[50];
    setTextColor("Black", "LYellow");
    cout << "Enter file name holding your SIC assembly instructions: ";
    cin >> fname;
    int i, mx;
    for(i = 0; fname[i] != '\0'; i++);
    mx = i;
    string txt = ".txt";
    int k = 0;
    for(; i < (mx+4); i++){
        fname[i] = txt[k++];
    }
    fname[i] = '\0';

    string line;
    system("CLS");
    try {
        ifstream code(fname);
        if(code.is_open()){
           while(getline(code, line)){
                program.push(line);
            }
            code.close();
            _program = program;
        }
        else {
            throw runtime_error("what()?: Parse/Read Error");
        }
    }
    catch(exception &e){
        setTextColor("Black", "LRed");
        cerr << "ERROR: Error reading file, try again." << nl;
        Sleep(1000);
        cerr << "SYSTEM: Parsing stopped...";
        Sleep(1000);
        return 1;
    }

    solve();
    int x;
    cin >> x;

    return 0;
}
