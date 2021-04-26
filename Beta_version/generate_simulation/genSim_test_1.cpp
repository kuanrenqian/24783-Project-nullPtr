#include "genSim.h"
using namespace std;

int main(){
    generate_simulation_files("m_0.bdf",0);

    FILE *fp1 = fopen("./output/simulation_input_journal0.jou", "r");
    FILE *fp2 = fopen("../generate_simulation/tests_resource_files/simulation_input_journal0.jou", "r");

    if (fp1 == NULL || fp2 == NULL)
    {
        return 1;
    }

    int state = compareFiles(fp1, fp2);
     // closing both file
    fclose(fp1);
    fclose(fp2);

    cout << state << endl;
    return state;
}
