#include "genSim.h"

int main(){
    vector <float> col = GUI_test2();
    float max_val = *max_element(col.begin(), col.end());
    if(max_val!=2){
        return 1;
    }
    cout << "WORKS" << endl;
    return 0;
}