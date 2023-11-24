#include <iostream>
#include <cstdlib>
#include <string>
#include "LP.h"

using namespace std;

string MENU = "commands: \n1. Insert problem \n2. Display problem \n3. Make Standard \n4. Compute dual \n5. Compute Simplex using Big M method \n6. Compute Simplex using 2 phase method \n7. Reset problem \n8. Exit \nEnter the command number: ";

void get_problem(lpProblem &p){
    cout << "problem information.\n";
    int variables_count, constraints_count, problem_type;
    float objective_function_constant;
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

    float *objective_function = new float[variables_count], *constraint_vector = new float[constraints_count];
    float **constraint_matrix = new float*[constraints_count];
    string *inequalities_type = new string[constraints_count];
    for(int i = 0; i < constraints_count; i++){
        constraint_matrix[i] = new float[variables_count];
    }
    cout << "Enter Objective Function: \n";
    for(int i = 0; i < variables_count; i++){
        cin >> objective_function[i];
    }
    p.setObjectiveFunction(objective_function);

    cout << "Constraints :\n";
    for(int i = 0; i < constraints_count; i++){
        for(int j = 0; j < variables_count; j++){
            cin >> constraint_matrix[i][j];
        }
        cin >> inequalities_type[i];
        cin >> constraint_vector[i];
    }
    cout << "Variables lower bound: \n";
    for(int i = 0; i < variables_count; i++){
        float lower_bound;
        cin >> lower_bound;
        if(lower_bound != 0){
            for(int j = 0; j < constraints_count; j++){
                constraint_vector[j] -= lower_bound * constraint_matrix[j][i];
            }
            //constant for objective function
        }
    }
    p.setProblemConstraints(constraint_matrix, inequalities_type, constraint_vector);
    //p.makeCanonical();
}

void press_key(){
    cout << "press Enter to continue ...";
    cin.get();
    cin.get();
}

int main(){
    lpProblem p, back_up;
    while(true){
        system("clear");
        cout << MENU;
        int command;
        cin >> command;
        if(command == 8){
            cout << "Bye\n";
            break;
        }
        system("clear");
        if(command == 1){
            get_problem(p);
            back_up = p;
            press_key();
        }
        else if(command == 2){
            p.displayProblem();
            press_key();
        }
        else if(command == 3){
            cout << "The standard form is :\n";
            p.makeStandardFrom();
            p.displayProblem();
            press_key();
        }
        else if(command == 4){
            cout << "The dual is :\n";
            p.computeDual().displayProblem("Y");
            press_key();
        }
        else if(command == 5){
            cout << "Result is : \n";
            p.computeSimplex_BigM();
            press_key();
        }
        else if(command == 6){
            cout << "Result is : \n";
            p.computeSimplex_2phase();
            press_key(); 
        }
        else if(command == 7){
            p = back_up;
            cout << "Problem reseted" << endl;
            p.displayProblem();
            press_key();
        }
        else{
            cout << "Invalid command!\n";
        }
    }
    return 0;
}