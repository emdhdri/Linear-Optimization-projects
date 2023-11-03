#include <iostream>
#include <string>
#include <vector>

using namespace std;


class lpProblem{
    //If the problem is a maximizing problem then problem_type is 1 an otherwise its value is 2.
    int variables_count, constraints_count, problem_type;
    int *objective_function;
    int **constraint_matrix;
    string *inequalities_type;
    int *constraint_vector;
    vector<int> nonzero_indexes;
public:
    lpProblem(){
        variables_count = 0;
        constraints_count = 0;
        problem_type = -1;
        objective_function = nullptr;
        constraint_matrix = nullptr;
        inequalities_type = nullptr;
        constraint_vector = nullptr;
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
    }
    void setProblemInfo(int _variables_count,int _constraints_count,int _problem_type){
        variables_count = _variables_count;
        constraints_count = _constraints_count; 
        problem_type = _problem_type;
    }

    void setObjectiveFunction(int *_objective_function){
        objective_function = _objective_function;
        _objective_function = nullptr;
        for(int i = 0; i < variables_count; i++){
            if(objective_function[i] != 0){
                nonzero_indexes.push_back(i);
            }
        }
    }

    void setProblemConstraints(int **_constraint_matrix, string *_inequalities_type, int *_constraint_vector){
        constraint_matrix = _constraint_matrix;
        inequalities_type = _inequalities_type;
        constraint_vector = _constraint_vector;
        _constraint_matrix = nullptr;
        _inequalities_type = nullptr;
        _constraint_vector = nullptr;

    }

    void displayProblem(string variable);
    void makeCanonical();
    lpProblem computeDual();

};

void lpProblem::displayProblem(string variable = "X"){
    cout << "--------------------------------------\n";
    if(problem_type == 1){
        cout << "Maximize: \n";
    }
    else{
        cout << "Minimize: \n";
    }
    for(int i = 0; i < nonzero_indexes.size(); i++){
        cout << "(" <<objective_function[i] << "*" << variable << i + 1 << ")";
        if(i < nonzero_indexes.size() - 1){
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

void lpProblem::makeCanonical(){
    for(int i = 0; i < constraints_count; i++){
        if(inequalities_type[i] == "="){
            inequalities_type[i] = "<=";
            constraints_count++;
            int **tmp_constraint_matrix = new int*[constraints_count], *tmp_constraint_vector = new int[constraints_count];
            for(int j = 0; j < constraints_count; j++){
                tmp_constraint_matrix[j] = new int[variables_count];
            }
            string *tmp_inequalities_type = new string[constraints_count];
            for(int j = 0; j < constraints_count - 1; j++){
                tmp_constraint_matrix[j] = constraint_matrix[j];
                tmp_inequalities_type[j] = inequalities_type[j];
                tmp_constraint_vector[j] = constraint_vector[j];
            }
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
    if(problem_type == 2){
        for(int i = 0; i < constraints_count; i++){
            for(int j = 0; j < variables_count; j++){
                constraint_matrix[i][j] *= -1;
            }
            inequalities_type[i] = ">=";
            constraint_vector[i] *= -1;
        }
    }
}

lpProblem lpProblem::computeDual(){
    lpProblem dual;
    int dual_problem_type;
    dual_problem_type = 1 ? problem_type == 2 : 2;
    dual.setProblemInfo(constraints_count, variables_count, dual_problem_type);
    int *dual_objective_function = new int[constraints_count];
    for(int i = 0; i < constraints_count; i++){
        dual_objective_function[i] = constraint_vector[i];
    }
    dual.setObjectiveFunction(dual_objective_function);
    int **dual_constraint_matrix = new int*[variables_count], *dual_constraint_vector = new int[variables_count];
    for(int i = 0; i < variables_count; i++){
        dual_constraint_matrix[i] = new int[constraints_count];
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