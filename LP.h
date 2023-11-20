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
    int *base, *is_base;
    vector<int> artificial_variables;
    bool isCanonical, isStandard;
public:
    lpProblem(){
        variables_count = 0;
        constraints_count = 0;
        problem_type = -1;
        objective_function = nullptr;
        constraint_matrix = nullptr;
        inequalities_type = nullptr;
        constraint_vector = nullptr;
        base = nullptr;
        is_base = nullptr;
        isCanonical = false;
        isCanonical = false;
    }
    ~lpProblem(){
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
        delete [] is_base;
        is_base = nullptr;
    }
    void setProblemInfo(int _variables_count,int _constraints_count,int _problem_type){
        variables_count = _variables_count;
        constraints_count = _constraints_count; 
        problem_type = _problem_type;
    }

    void setObjectiveFunction(float *_objective_function){
        objective_function = _objective_function;
        _objective_function = nullptr;
        /*for(int i = 0; i < variables_count; i++){
            if(objective_function[i] != 0){
                nonzero_indexes.push_back(i);
            }
        }*/
    }

    void setProblemConstraints(float **_constraint_matrix, string *_inequalities_type, float *_constraint_vector){
        constraint_matrix = _constraint_matrix;
        inequalities_type = _inequalities_type;
        constraint_vector = _constraint_vector;
        _constraint_matrix = nullptr;
        _inequalities_type = nullptr;
        _constraint_vector = nullptr;

    }

    void displayProblem(string variable);
    void makeCanonical(bool complete_canonical);
    void makeStandardFrom();
    void computeSimplex_BigM(float M);
    pair<float, vector<float>> computeSimplex_TwoPhase();
    lpProblem computeDual();
    float compute_cost(int index);
    void change_row(int row1, int row2, float ratio);
};

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
void lpProblem::computeSimplex_BigM(float M = 100000){
    float result_values[variables_count] = {};
    //prepare problem and set artificial variables coefficient to M
    for(auto artificial_variable: artificial_variables){
        objective_function[artificial_variable] = M;
    }
    //if problem is a minimizing problem then we miltiply objective funtion with -1 and calculate maximum
    if(problem_type != 1){
        for(int i = 0; i < variables_count; i++){
            objective_function[i] *= -1;
        }
    }
    //iterate over states and terminate when there is no non basic variable 
    //that cannont increase objective function
    while(true){
        //find best choice between non basic variables to make it basic variable
        float max_cost = compute_cost(0);
        int max_cost_index = 0;
        for(int i = 0; i < variables_count; i++){
            float ith_cost = compute_cost(i);
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
            float minimum_ratio = M;
            int minimum_ratio_index = 0;
            for(int i = 0; i < constraints_count; i++){
                if(constraint_matrix[i][max_cost_index] > 0){
                    float ratio = constraint_vector[i] / constraint_matrix[i][max_cost_index];
                    if(ratio < minimum_ratio){
                        minimum_ratio = ratio;
                        minimum_ratio_index = i;
                    }
                }
            }

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
            float z = 0;
            for(int i = 0; i < constraints_count; i++){
                z += objective_function[base[i]] * constraint_vector[i];
                result_values[base[i]] = constraint_vector[i];
            }
            if(problem_type != 1){
                z *= -1;
            }
            cout << "Value is :" << z << endl;
            for(int i = 0; i < variables_count; i++){
                cout << "X" << i + 1 << ": " << result_values[i] << endl; 
            }
            break;
        }
    }
}