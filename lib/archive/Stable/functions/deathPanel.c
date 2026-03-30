/*------------------------------------------------------------------------------ +-
PROBABILISTIC DEATH OF AN OCCUPIED CELL

This function fetches the likelihood of death for a given species in a given environment from the corresponding element in
'deathProbs' and performs a Bernoulli trial to determine if the inhabitant of the focal cell survives to the next time step.

ENTRY:      1.  'cell' is the species value in the cell before the death trial.  'cell' is numbered from 1 to 'n_species'.
            2.  'resValue' is the value of the resource in the cell.
            3.  Each row of the 'deathProbs' matrix corresponds to the identity of 
                a species (beginning with 1), while columns represent the possible values of the resource (beginning with 0).  
                Each entry is the probability that the given species occupying the 
                focal cell will die in a time step given the corresponding resource level.
                                
EXIT:       1.  'deathPanel' returns the species value for the cell after the death trial. Possible values are 'cell' or 0. 

FUNCTIONS:  1.  dsfmt_genrand_close_open()

*/

int deathPanel(
  unsigned char cell,
  unsigned char resValue,
  float         **deathProbs
  )
{

   if(dsfmt_genrand_close_open(&dsfmt) < deathProbs[cell-1][resValue])
                                                      // If a randomly drawn float (between 0 and 1) is less than
                                                      // the value of the corresponding death probability
                                                      // then the cell is cleared.
    return(0);
  else
    return(cell);                                     // Otherwise the value of the cell is returned.
}  
