/*-------------------------------------------------------------------------------
READ PARAMETER VALUES.

Parameters for a given run of the program are stored in a text file of two columns. 
The first column contains parameter names, the second column containss the corresponding
values.  The first column must not be indented. Columns are separated by white space
(one or more tabs or spaces). 

Here is an excerpt of a sample file:

    gridSize	  10
    n_resource	 1
    n_species	   2

This function parses the configuration file and returns the value of specified parameter.

ENTRY:  'cfgFile' points to the configuration file.
        'arg' points the name of the parameter to retrieve.

EXIT:   'readCfg()' returns the value of the parameter specified on entry.

        
*/

char* readCfg(char *cfgFile, char *arg, bool print)
{
  int i = 0, j = 0;                              // Initialize loop variables.
  int argLength = strlen(arg);                   // Get the number of characters in the parameter name.
  char line1[LINE_LENGTH] = {0};                 // Create a string to hold the text in a line of the config file.
  char paramName[LINE_LENGTH] = {0};             // Create a string to hold the name of the parameter in 'line1'.
  static char paramValue[LINE_LENGTH] = {0};     // I get a compiler error if I don't make this static!
  
  FILE *configFile;

  configFile = fopen(cfgFile, "r");              // Open the configuration file for reading.

  for(i = 0; i < FILE_LENGTH; i++){              // Loop through each line of cfgFile:
    fgets(line1, LINE_LENGTH, configFile);       // Read line i, store it in 'line1'.
    strcpy(paramName, line1);                    // Copy 'line1' to 'paramName'.
    for(j = 0; j < LINE_LENGTH; j++)             // Loop through the characters of 'paramName'.
      if(paramName[j] <= ' ') paramName[j] = '\0';  // Insert a string-ending character at the first white space
    if(!strcmp(paramName, arg)) break;           // If 'paramName' and 'arg' match, stop the loop.
    } //for(i)

  fclose(configFile);                            // Close cfgFile.
    
  for(i = argLength; line1[i] <= ' '; i++);      // Bypass the white space between columns 1 and 2.
  
  j = 0;                                         // Reset j.
  
  for(i = i; i < LINE_LENGTH; i++){              // Loop through the second column of 'line1'.
    paramValue[j] = line1[i];                    // Copy the parameter value into 'paramValue'.
    if(paramValue[j] <= ' '|paramValue[j] <= '\n'){   // If whitespace, or the line-ending character is reached:
      paramValue[j] = '\0';                      // Substitute a string-ending character for the end-of-line character
      break;                                     // and end the loop.
      }
    j++;                                         // Incrememt j.
    } //for(i)
  
  return(paramValue);                            // Return the value of the parameter.

} // readCfg()



