/*------------------------------------------------------------------------------- +-
This function generates a random float between 0 and 1 using rand()

ENTRY:   1. The random number generator has been initialized (in main loop, requires time.h or a 
            user-provided random seed).

EXIT:    1. 'randomFloat' returns a uniformly distributed random number greater or
            equal to 0 and less than 1.

*/
float randomFloat()
{
      return (float)rand()/(float)(RAND_MAX);    // Divide a randomly chosen integer by the maximum
                                                     // integer size.      
                                                     
                                                     // consider switching to double
                                                     
                                                     
}


