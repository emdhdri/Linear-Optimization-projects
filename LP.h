#include <iostream>
#include <string>
#include <vector>

using namespace std;



class lpProblem{
    //If the problem is a maximizing problem then problem_type is 1 an otherwise its value is 2.
    int variables_count, constraints_count, problem_type;
    float *objective_function;
    float **constraint_matrix;
    string *inequalities_type;
    float *constraint_vector;
    int *base;
    vector<int> artificial_variables;
    bool isCanonical, isStandard;
    float *results;
    float Z;
    bool solved;
public:

    lpProblem();
    ~lpProblem();
    void setProblemInfo(int _variables_count,int _constraints_count,int _problem_type);
    void setObjectiveFunction(float *_objective_function);
    void setProblemConstraints(float **_constraint_matrix, string *_inequalities_type, float *_constraint_vector);
    void displayProblem(string variable);
    void makeCanonical(bool complete_canonical);
    void makeStandardFrom();
    void solve(bool second);
    void computeSimplex_BigM(float M);
    void computeSimplex_2phase();
    lpProblem computeDual();
    float compute_cost(int index);
    void change_row(int row1, int row2, float ratio);
    void operator=(const lpProblem& C);
};
lpProblem::lpProblem(){
    variables_count = 0;
    constraints_count = 0;
    problem_type = -1;
    objective_function = nullptr;
    constraint_matrix = nullptr;
    inequalities_type = nullptr;
    constraint_vector = nullptr;
    base = nullptr;
    isCanonical = false;
    isCanonical = false;
    solved = false;
}
void lpProblem::operator=(const lpProblem &P){
    variables_count = P.variables_count;
    constraints_count = P.constraints_count;
    problem_type = P.problem_type;
    if(objective_function != nullptr){
        delete [] objective_function;
    }
    objective_function = new float[variables_count];
    for(int i = 0; i < variables_count; i++){
        objective_function[i] = P.objective_function[i];
    }
    if(constraint_matrix != nullptr){
        for(int i = 0; i < constraints_count; i++){
            if(constraint_matrix[i] != nullptr){
                delete [] constraint_matrix[i];
            }
        }
        delete [] constraint_matrix;
    }
    if(inequalities_type != nullptr){
        delete [] inequalities_type;
    }
    if(constraint_vector != nullptr){
        delete [] constraint_vector;
    }
    constraint_matrix = new float*[constraints_count];
    inequalities_type = new string[constraints_count];
    constraint_vector = new float[constraints_count];
    for(int i = 0; i < constraints_count; i++){
        constraint_matrix[i] = new float[variables_count];
        for(int j = 0; j < variables_count; j++){
            constraint_matrix[i][j] = P.constraint_matrix[i][j];
        }
        inequalities_type[i] = P.inequalities_type[i];
        constraint_vector[i] = P.constraint_vector[i];
    }
    base = nullptr;
    artificial_variables = P.artificial_variables;
    isCanonical = P.isCanonical;
    isStandard = P.isStandard;
}

lpProblem::~lpProblem(){
    delete [] objective_function;
    for(int i = 0; i < constraints_count; i++){
        delete [] constraint_matrix[i];
        constraint_matrix[i] = nullptr;
    }
    delete [] constraint_matrix;
    constraint_matrix = nullptr;
    delete [] inequalities_type;
    inequalities_type = nullptr;
    delete [] constraint_vector;
    constraint_vector = nullptr;
    delete [] base;
    base = nullptr;
}
void lpProblem::setProblemInfo(int _variables_count,int _constraints_count,int _problem_type){
    variables_count = _variables_count;
    constraints_count = _constraints_count; 
    problem_type = _problem_type;
}

void lpProblem::setObjectiveFunction(float *_objective_function){
    objective_function = _objective_function;
    _objective_function = nullptr;
}

void lpProblem::setProblemConstraints(float **_constraint_matrix, string *_inequalities_type, float *_constraint_vector){
    constraint_matrix = _constraint_matrix;
    inequalities_type = _inequalities_type;
    constraint_vector = _constraint_vector;
    _constraint_matrix = nullptr;
    _inequalities_type = nullptr;
    _constraint_vector = nullptr;

}

void lpProblem::displayProblem(string variable = "X"){
    cout << "--------------------------------------\n";
    if(problem_type == 1){
        cout << "Maximize: \n";
    }
    else{
        cout << "Minimize: \n";
    }
    for(int i = 0; i < variables_count; i++){
        cout << "(" << objective_function[i] << "*" << variable << i + 1 << ")";
        if(i < variables_count - 1){
            cout << " + ";
        }
    }
    cout << "\nSubject to: \n";
    for(int i = 0; i < constraints_count; i++){
        for(int j = 0; j < variables_count; j++){
            cout << "(" << constraint_matrix[i][j] << "*" << variable << j + 1 << ") ";
            if(j < variables_count - 1){
                cout << "+ ";
            }
        }
        cout << inequalities_type[i] << " ";
        cout << constraint_vector[i] << "\n";
    }
    for(int i = 0; i < variables_count; i++){
        cout << variable << i + 1;
        if(i < variables_count - 1){
            cout << ", ";
        }
    }
    cout << " >= " << 0 << "\n";

    cout << "--------------------------------------\n";
}

void lpProblem::makeCanonical(bool complete_canonical = true){
    for(int i = 0; i < constraints_count; i++){
        if(inequalities_type[i] == "=" && complete_canonical){
            inequalities_type[i] = "<=";
            constraints_count++;
            float **tmp_constraint_matrix = new float*[constraints_count], *tmp_constraint_vector = new float[constraints_count];
            string *tmp_inequalities_type = new string[constraints_count];
            for(int j = 0; j < constraints_count - 1; j++){
                tmp_constraint_matrix[j] = constraint_matrix[j];
                tmp_inequalities_type[j] = inequalities_type[j];
                tmp_constraint_vector[j] = constraint_vector[j];
            }
            tmp_constraint_matrix[constraints_count - 1] = new float[variables_count];
            for(int j = 0; j < variables_count; j++){
                tmp_constraint_matrix[constraints_count - 1][j] = -constraint_matrix[i][j];
            }
            tmp_inequalities_type[constraints_count - 1] = "<=";
            tmp_constraint_vector[constraints_count - 1] = -constraint_vector[i];
            constraint_matrix = tmp_constraint_matrix;
            inequalities_type = tmp_inequalities_type;
            constraint_vector = tmp_constraint_vector;
            tmp_constraint_matrix = nullptr;
            tmp_inequalities_type = nullptr;
            tmp_constraint_vector = nullptr;
        }
        else if(inequalities_type[i] == ">" || inequalities_type[i] == ">="){
            for(int j = 0; j < variables_count; j++){
                constraint_matrix[i][j] *= -1;
            }
            inequalities_type[i] = "<=";
            constraint_vector[i] *= -1;
        }
        else if(inequalities_type[i] == "<"){
            inequalities_type[i] = "<=";
        }
    }
    /*if(problem_type == 2){
        for(int i = 0; i < constraints_count; i++){
            for(int j = 0; j < variables_count; j++){
                constraint_matrix[i][j] *= -1;
            }
            inequalities_type[i] = ">=";
            constraint_vector[i] *= -1;
        }
    }*/
    isCanonical = true;
}

lpProblem lpProblem::computeDual(){
    if(!isCanonical){
        makeCanonical();
    }
    lpProblem dual;
    int dual_problem_type;
    dual_problem_type = 1 ? problem_type == 2 : 2;
    dual.setProblemInfo(constraints_count, variables_count, dual_problem_type);
    float *dual_objective_function = new float[constraints_count];
    for(int i = 0; i < constraints_count; i++){
        dual_objective_function[i] = constraint_vector[i];
    }
    dual.setObjectiveFunction(dual_objective_function);
    float **dual_constraint_matrix = new float*[variables_count], *dual_constraint_vector = new float[variables_count];
    for(int i = 0; i < variables_count; i++){
        dual_constraint_matrix[i] = new float[constraints_count];
    }
    string *dual_inequalities_type = new string[variables_count];
    for(int i = 0; i < variables_count; i++){
        for(int j = 0; j < constraints_count; j++){
            dual_constraint_matrix[i][j] = constraint_matrix[j][i];   
        }
        dual_constraint_vector[i] = objective_function[i];
        if(problem_type == 1){
            dual_inequalities_type[i] = ">=";
        }
        else{
            dual_inequalities_type[i] = "<=";
        }
    }
    dual.setProblemConstraints(dual_constraint_matrix, dual_inequalities_type, dual_constraint_vector);
    return dual;

}

void lpProblem::makeStandardFrom(){
    if(isCanonical == false){
        makeCanonical(false);
    }
    int start_index[constraints_count];
    int slack_count = 0;
    base = new int[constraints_count];
    for(int i = 0; i < constraints_count; i++){
        start_index[i] = slack_count; 
        slack_count++;
        if(inequalities_type[i] == "="){
            artificial_variables.push_back(variables_count + slack_count - 1);
        }
        if(constraint_vector[i] < 0 && inequalities_type[i] != "="){
            slack_count++;
            artificial_variables.push_back(variables_count + slack_count - 1);
        }
        base[i] = variables_count + slack_count - 1;
        
    }
    float *tmp_objective_function = new float[variables_count + slack_count];
    for(int i = 0; i < variables_count; i++){
        tmp_objective_function[i] = objective_function[i];
    }
    for(int i = 0; i < slack_count; i++){
        tmp_objective_function[variables_count + i] = 0;
    }
    delete [] objective_function;
    objective_function = tmp_objective_function;
    tmp_objective_function = nullptr;
    float **tmp_constraint_matrix = new float*[constraints_count];
    for(int i = 0; i < constraints_count; i++){
        tmp_constraint_matrix[i] = new float[variables_count + slack_count];
    }
    for(int i = 0; i < constraints_count; i++){
        int sign = (constraint_vector[i] < 0) ? -1 : 1;

        for(int j = 0; j < variables_count; j++){
            tmp_constraint_matrix[i][j] = sign * constraint_matrix[i][j];
        }
        for(int j = 0; j < start_index[i]; j++){
            tmp_constraint_matrix[i][variables_count + j] = 0;
        }
        int remain_index;
        if(inequalities_type[i] != "="){
            tmp_constraint_matrix[i][variables_count + start_index[i]] = sign * 1;
            remain_index = variables_count + start_index[i] + 1;
            if(constraint_vector[i] < 0){
                tmp_constraint_matrix[i][remain_index] = sign * -1;
                remain_index++;
            }
        }
        else{
            tmp_constraint_matrix[i][variables_count + start_index[i]] = 1;
            remain_index = variables_count + start_index[i] + 1;
        }
        for(int j = remain_index; j < variables_count + slack_count; j++){
            tmp_constraint_matrix[i][j] = 0;
        }
        constraint_vector[i] *= sign;
        inequalities_type[i] = "=";
    }
    for(int i = 0; i < constraints_count; i++){
        delete [] constraint_matrix[i];
        constraint_matrix[i] = nullptr;
    }
    delete [] constraint_matrix;
    constraint_matrix = tmp_constraint_matrix;
    tmp_constraint_matrix = nullptr;
    variables_count += slack_count;
    isStandard = true;   
}


void lpProblem::change_row(int row1, int row2, float ratio){
    for(int i = 0; i < variables_count; i++){
        constraint_matrix[row2][i] += ratio * constraint_matrix[row1][i];
    }
    constraint_vector[row2] += ratio * constraint_vector[row1];
}
float lpProblem::compute_cost(int index){
    float cost = objective_function[index];
    for(int i = 0; i < constraints_count; i++){
        cost -= constraint_matrix[i][index] * objective_function[base[i]];
    }
    return cost;
}

void lpProblem::solve(bool second = false){
    float result_values[variables_count] = {};
    //if problem is a minimizing problem then we miltiply objective funtion with -1 and calculate maximum
    if(problem_type != 1){
        for(int i = 0; i < variables_count; i++){
            objective_function[i] *= -1;
        }
    }
    if(isStandard == false){
        makeStandardFrom();
    }
    while(true){
        displayProblem();
        //find best choice between non basic variables to make it basic variable
        float max_cost = compute_cost(0);
        int max_cost_index = 0;
        for(int i = 0; i < variables_count; i++){
            if(second){
                int flg = 0;
                for(auto x : artificial_variables){
                    if(i == x){
                        flg = 1;
                        break;
                    }
                }
                if(flg){
                    continue;
                }
            }
            float ith_cost = compute_cost(i);
            cout << "X" << i + 1 << " cost is : " << ith_cost << endl;
            if(ith_cost > max_cost){
                max_cost = ith_cost;
                max_cost_index = i;
            }
        }

        //if max_cost <= 0 then this means we cannont increase objective function anymore
        if(max_cost > 0){
            int flag = 0;
            //find the basic variable that should leave base by finding the
            //minimum ratio between RHS and factors.
            float minimum_ratio = 1e8;
            int minimum_ratio_index = 0;
            for(int i = 0; i < constraints_count; i++){
                if(constraint_matrix[i][max_cost_index] > 0){
                    flag = 1;
                    float ratio = constraint_vector[i] / constraint_matrix[i][max_cost_index];
                    if(ratio < minimum_ratio){
                        minimum_ratio = ratio;
                        minimum_ratio_index = i;
                    }
                }
            }
            if(flag == 0){
                int variable_sign = 1 ? objective_function[max_cost_index] > 0 : -1;
                if(problem_type != 1){
                    variable_sign *= -1;
                }
                cout << variable_sign << " INF";
                break;
            }
            cout << "X" << base[minimum_ratio_index] + 1 << " leaves the base and X" << max_cost_index + 1 << " comes in.\n";
            //make pivot cell value 1 by deviding contraint to its value
            float pivot = constraint_matrix[minimum_ratio_index][max_cost_index];
            for(int i = 0; i < variables_count; i++){
                constraint_matrix[minimum_ratio_index][i] /= pivot;
            }
            constraint_vector[minimum_ratio_index] /= pivot;

            //make the non basic variable, a basic variable by adding a multiple of pivot row 
            //to other rows and making pivot column 0 in other rows
            for(int i = 0; i < constraints_count; i++){
                if(constraint_matrix[i][max_cost_index] != 0 && i != minimum_ratio_index){
                    float ratio = constraint_matrix[i][max_cost_index] / constraint_matrix[minimum_ratio_index][max_cost_index];
                    change_row(minimum_ratio_index, i, -(ratio));
                }
            }
            base[minimum_ratio_index] = max_cost_index;
        }
        else{
            int artificial_var_flag = 0;
            for(auto artificial_var_index: artificial_variables){
                if(result_values[artificial_var_index] != 0){
                    artificial_var_flag = 1;
                    cout << "This problem has no solution!" << endl;
                }
            }
            if(artificial_var_flag){
                break;
            }
            solved = true;
            float z = 0;
            for(int i = 0; i < constraints_count; i++){
                z += objective_function[base[i]] * constraint_vector[i];
                result_values[base[i]] = constraint_vector[i];
            }
            if(problem_type != 1){
                z *= -1;
            }
            Z = z;
            results = new float[variables_count];
            cout << "Value is :" << z << endl;
            for(int i = 0; i < variables_count; i++){
                results[i] = result_values[i];
                cout << "X" << i + 1 << ": " << result_values[i] << endl; 
            }
            break;
        }
    }
}

void lpProblem::computeSimplex_BigM(float M = 100000){

    //prepare problem and set artificial variables coefficient to M
    for(auto artificial_variable: artificial_variables){
        if(problem_type == 2)
            objective_function[artificial_variable] = M;
        else
            objective_function[artificial_variable] = -M;
    }

    //call solve function
    solve();
}

void lpProblem::computeSimplex_2phase(){
    lpProblem phase1;
    phase1.variables_count = variables_count;
    phase1.constraints_count = constraints_count;
    phase1.constraint_matrix = new float*[constraints_count];
    for(int i = 0; i < constraints_count; i++){
        phase1.constraint_matrix[i] = new float[variables_count];
    }
    phase1.constraint_vector = new float[constraints_count];
    phase1.inequalities_type = new string[constraints_count];
    for(int i = 0; i < constraints_count; i++){
        //constraint_matrix[i] = new float[variables_count];
        for(int j = 0; j < variables_count; j++){
            phase1.constraint_matrix[i][j] = constraint_matrix[i][j];

        }
        phase1.inequalities_type[i] = inequalities_type[i];
        phase1.constraint_vector[i] = constraint_vector[i];
    }
    phase1.objective_function = new float[variables_count];
    for(int i = 0; i < variables_count; i++){
        phase1.objective_function[i] = 0;
    }
    for(auto artificial_var_index: artificial_variables){
        phase1.objective_function[artificial_var_index] = 1;
    }
    phase1.problem_type = 2;
    phase1.base = new int[constraints_count];
    for(int i = 0; i < constraints_count; i++){
        phase1.base[i] = base[i];
    }
    phase1.isStandard = true;
    phase1.solve();
    if(phase1.solved && phase1.Z == 0){
        displayProblem();
        cout << "Phase two started : \n";
        for(auto index : artificial_variables){
            objective_function[index] = 0;
        }
        for(int i = 0; i < constraints_count; i++){
            for(int j = 0; j < variables_count; j++){
                constraint_matrix[i][j] = phase1.constraint_matrix[i][j];
            }
            inequalities_type[i] = phase1.inequalities_type[i];
            constraint_vector[i] = phase1.constraint_vector[i];
        }
        base = new int[constraints_count];
        for(int i = 0; i < constraints_count; i++){
            base[i] = phase1.base[i];
        }
        displayProblem();
        solve(true);
    }
}
