#include "genSim.h"
using namespace std;

int main(){
   
    float R,G,B;
    parula_colormap(0.5, 1, R, G, B);

    float RR = 0.1466;
    float GG = 0.7597;
    float BB = 0.6797;

    if(R!=RR||G!=GG||B!=BB){
        cout << "failed" << endl;
        return 1;
    }
    cout << "works" << endl;

    return 0;
}