# Stochastic Cellular Automaton Model With Disturbance

The following is a description of the model in the Overview, Design Concepts, and Details (ODD) protocol format as specified in <a href="https://www.jasss.org/23/2/7.html#grimm2006" target="_blank"> Grimm et al. (2020)</a>.

A playlist of videos of model simulations is <a href="https://youtube.com/playlist?list=PLjS0IFRRz3mNhVaCpMFlCQpRQIepPGy9o&si=m6_zLaqh2qwIVUjd">here</a>.

## 1. Purpose and Patterns

A stochastic cellular automaton model in which species colonize empty cells, species compete to displace one another from occupied cells, and occupied cells have a chance to be cleared by random disturbances.


## 2. Entities, State Variables, and Scales

* Field:  This is the grid on which species live.  It has wrapping borders, i.e. it is a torus.  The user specifies the field size in the model configuration file.
* Habitat: This is a grid of the same dimensions of the field.  It specifies, for each cell, the habitat ID.  Species may have different colonization and competition parameters on different habitats.
* Species: Species may occupy cells in the field.  They have parameters that specify the probabilities with which they can occupy empty cells (colonization probability) and displace other species (competition/displacement probabilities). Competition parameters are defined pairwise, that is each species has a certain probability of displacing another species.  This probability can be different for different species pairs such species A may be a strong competitor against species B but a weak competitor against species C.  Species also have a probability of randomly dying (death probability) in each time step.  Species are represented in the field by integer codes starting with 1.  A code of 0 indicates that a cell is empty.
* Temporal Scales: The model proceeds in discrete time steps.  The step process is simultaneous for all cells in that the submodels are applied to all cells at once, not sequentially.  The user specifies the length of the simulation.  Simulation states may be saved at the final time step and resumed later.
* Spatial scales: Species interact with empty spaces and each other locally by sensing other cells in the 8 immediately adjacent cells in the Moore neighborhood.  The user speciies the number of rows and columns in the field.
* Required parameters:
  * Grid size - the number of rows and columns in the field.
  * The number of species in the simulation.
  * The number of habitats in the simulation.
  * Flag for whether or not to apply the disturbance submodel.
  * The mean percent of patches to be disturbed in each time step.
  * The radius of the disturbance patches.
  * Random seed for the random number generators.  If a value of 0 is provided, the RNG is initialized using the current time.
  * Field initialization file
  * Habitat file
  * Filenames of the colonization, competition, and death parameters.
  * Census output filename.
* Optional parameters:
  * Output file for the state of the field at each time step
  * Output file to contain the state of the field at the end of the simulation (useful if one wants to resume a simulation later).
  * Field resume file (allows the user to resume a simulation from a previous state).
  * Filaname to save the final state of the simulation.
  * Filename to save output images of the field at each time step
  * Colormap for the images.  If not supplied, and the user wishes to save images, a random colormap will be created.

## 3. Process Overview and Scheduling

Each time step the following submodels are applied in order:

1. Colonization Submodel:  Allows species to colonize empty cells.
2. Displacement/Competition Submodel: Species compete to displace one another.
3. Species Death: Occupied cells have a species-specific probability of becoming unoccupied.
4. Disturbance: Cells are randomly disturbed (emptied).
5. Cenusus: Counts the number of cells occupied by each species after the submodels have been applied.

## 4. Design Concepts

## 5. Initialization

The simulation begins by reading in a field file which specifies the initial state of each cell.  Cells may initially be empty (value 0) or occupied by a species (integer values of 1 or more).
The habitat file specifies the habitat ID for each cell.

Parameter files specifying the species' colonization, competition, and death probabilities are read at the initialization of simulation.

## 6. Input Data

There are 6 required input files for a simulation:

1. Model configuration file:  This lists the general parameters of the simulation run, including the names of files that store species parameters.
2. Colonization probabilities file:  This file contains the probabilities of colonization of empty sites for each species/habitat combination.
3. Death probabilities file:  This file contains the probabilities of death for each species/habitat combination.
4. Displacement/Competition file:  This file contains the probabilties of displacement for each species pair in each habitat.
5. Field file: This file contains the initial state of the field.
6. Habitate file: This file contains the habitat type for each cell in the field.

All files are delimited and may use either spaces or commas as the delimiters.

## 7. Submodels

### Colonization

This submodel allows species to colonize an adjacent empty cell.  For each empty cell, the identity of the species in the cells in the Moore neighborhood are identified.  Next, the probabiltiy that none of the surrounding species colonize is calculated as the product 1 - p(colonize) for all 8 neighbors.  If the value of a randomly generated decimal number between 0 and 1 is less than this value, a random neighbor is chosen using a weighted random selection procedure in which neighbors are weighted by their colonization probabilities.

### Displacement/Competition

This submodel determines the outcome of competition for an occupied cell.  First, the species in the Moore neighbood are identified.  Next the probability that none of the competing species colonize the occupied cell is calculated as the product of 1 - p(displace) for all 8 neighbors.  If a randomly generated decimal number is less than this value, a random neighbor is selected as the winner using a weighted random selection procedure in which the neighbors are weighted by their displacement probabilities.

Displacement probabilities are pairwise and habitat-specific.  That is each competing neighbor species has a certain probability of displacing the species occupying the central cell.  These displacement probabilities do not need to be symmetrical.  The probabiltiiy of species A displacing species B does not have to be the same as the probability of species B displacing species B, and usually these probabilities will be different.  Pairwise displacement probabilities can vary by habitat.

### Death

In this submodel, each occupied cell has some probability of being randomly emptied.  Each species has a habitat-specific probability of dying in each round, whereby the cell it occupies is emptied and its species code is replaced by a 0, indicating an empty cell.

### Disturbance

In this model, occupied cells falling underneath a circular disturbance patch footprint are cleared.  The size and shape of the footprint is calculated during model initialization based on the disturbance patch radius parameter.  The mean number of disturbance patches per time step is calculated based on the number of cells in the footprint and the parameter specifying the mean percentage of cells to be disturbed each time step.

First, a Poisson-distributed random number is generated with lambda equal to the mean number of disturbance patches per step.  If this number is greater than 0, for each disturbance patch a random row and column are chosen.  This cell serves as the center of the circular footprint, and all cells falling under the footprint are then emptied.

### Census

After the other submodels have been applied, the number of empty cells and the number of cells occupied by each species are calculated and saved to an output file.
