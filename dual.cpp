#include <iostream>
#include <string>
#include "LP.h"

using namespace std;

string MENU = "commands: \n1. Compute Dual \n2. Exit \nEnter the command number: ";

void LP_dual(){
    lpProblem p;

    cout << "problem information.\n";
    int variables_count, constraints_count, problem_type;
    while(true){
        cout << "Enter decision variables count: ";
        cin >> variables_count;
        cout << "Enter constraints count: ";
        cin >> constraints_count;
        cout << "Problem type:\n1. maximize\n2. minimize\nchoose problem type: ";
        cin >> problem_type;
        if(variables_count < 0 || constraints_count < 0 || (problem_type != 1 && problem_type != 2)){
            cout << "Invalid information!\n";
        }
        else{
            p.setProblemInfo(variables_count, constraints_count, problem_type);
            break;
        }
    }

    int *objective_function = new int[variables_count], *constraint_vector = new int[constraints_count];
    int **constraint_matrix = new int*[constraints_count];
    string *inequalities_type = new string[constraints_count];
    for(int i = 0; i < constraints_count; i++){
        constraint_matrix[i] = new int[variables_count];
    }
    cout << "Enter Objective Function: \n";
    for(int i = 0; i < variables_count; i++){
        cin >> objective_function[i];
    }
    p.setObjectiveFunction(objective_function);

    cout << "Constraints \n";
    for(int i = 0; i < constraints_count; i++){
        cout << "Enter constraint number " << i + 1 << "\n";
        for(int j = 0; j < variables_count; j++){
            cin >> constraint_matrix[i][j];
        }
        cin >> inequalities_type[i];
        cin >> constraint_vector[i];
    }
    p.setProblemConstraints(constraint_matrix, inequalities_type, constraint_vector);
    p.makeCanonical();
    p.displayProblem();
}

int main(){
    while(true){
        cout << MENU;
        int command;
        cin >> command;
        if(command == 1){
            LP_dual();
        }
        else if(command == 2){
            cout << "Bye.\n";
            break;
        }
        else{
            cout << "Invalid command!\n";
        }
    }
    return 0;
}