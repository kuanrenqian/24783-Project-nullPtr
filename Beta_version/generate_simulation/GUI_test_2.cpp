#include "genSim.h"

int main(){
    vector <float> col = GUI_test2();
    float max_val = *max_element(col.begin(), col.end());
    if(max_val!=45.4436){
        return 1;
    }
    return 0;
}