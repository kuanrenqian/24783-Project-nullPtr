#include "genSim.h"

int main(){
    int vtx_size = GUI_test1();
    if(vtx_size!=9348){
        return 1;
    }
    cout << "WORKS" << endl;
    return 0;
}