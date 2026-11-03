#include <iostream>
#include <Pythia8/Pythia.h>

int main(){
  Pythia8::Pythia pythia;
  

  pythia.readString("Beams:idA = 2212");
  pythia.readString("Beams:idB = 2212");
  pythia.readString("Beams:eCM = 13600.");
  pythia.readString("SoftQCD:inelastic = on");
  pythia.readString("Random:setSeed = on");
  pythia.readString("Random:seed = 12345");

    
  pythia.readString("Next:numberShowEvent = 0");


  if (!pythia.init()) {
    std::cerr << "Pythia init failed" << std::endl;
    return 1;
  }

  const int nEvents = 5;
  int nAccepted = 0;

  while (nAccepted < nEvents) {
    if (!pythia.next()) {
      continue;
    }
   
    int nFinal = 0;
    for (int i = 0; i < pythia.event.size(); i++){
      if (pythia.event[i].isFinal()){
        nFinal++;
      }
    }

    std::cout << "Event " << nAccepted << ": " << nFinal << " final particles" << std::endl;
    nAccepted++;

}
}