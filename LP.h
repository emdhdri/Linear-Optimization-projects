class lpProblem{
    //If the problem is a maximizing problem then problem_type is 1 an otherwise its value is 0.
    int variables_count, constraints_count, problem_type;
    int *objective_function;
    int **constraints;
public:
    lpProblem(){
        variables_count = 0;
        constraints_count = 0;
        problem_type = -1;
        objective_function = nullptr;
        constraints = nullptr;
    }
    void setProblemInfo(int _variables_count,int _constraints_count,int _problem_type){
        variables_count = _variables_count;
        constraints_count = _constraints_count; 
        problem_type = _problem_type;
    }
    void setObjectiveFunction(int *_objective_function){
        objective_function = _objective_function;
        _objective_function = nullptr;
    }
    void setProblemConstraints(int **_constraints){
        constraints = _constraints;
        _constraints = nullptr;
    }
    void displayProblem(){
        
    }

}