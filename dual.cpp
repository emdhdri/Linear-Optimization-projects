#include <iostream>
#include <string>

using namespace std;

class lpProblem{
    int variables_count, constraints_count, problem_type;
    
}

string MENU = "commands: \n1. Compute Dual \n2. Exit \nEnter the command number: ";

void LP_dual(){
    int variables_count, constraints_count, problem_type;
    cout << "Enter decision variables count: ";
    cin >> variables_count;
    cout << "Enter constraints count: ";
    cin >> constraints_count;
    cout << "If the problem is maximize problem enter 1 otherwise enter 0: ";
    cin >> problem_type;


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