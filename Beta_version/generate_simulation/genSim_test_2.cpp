#include "genSim.h"
using namespace std;

int main(){
   
    // run_simulation_files(0);
    FILE *fp1 = fopen(".\\output\\test_result_0.txt", "r");
    FILE *fp2 = fopen("..\\generate_simulation\\tests_resource_files\\test_result_0.txt", "r");
  
    if (fp1 == NULL || fp2 == NULL)
    {
       printf("Error : Files not open");
       return 1;
    }
  
    compareFiles(fp1, fp2);
  
    // closing both file
    fclose(fp1);
    fclose(fp2);
    cout << 0 << endl;
    return 0;
}
