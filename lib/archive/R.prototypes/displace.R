# Two species, two environments.
# Rows act on columns.
# species x acting on species x has probability 0.5, so that the greater the number of 
# self neighbors, the lower the chance of displacement by another type.

displace.matrix = rbind(
	c(0, 0.5, 0.3), c(0.5, 0, 0.4), c(0.2, 0.3, 0), 
	c(0.5, 0.5, 0.3), c(0.9, 0.5, 0.4), c(0.1, 0.3, 0.5))
displace.matrix
displace.vector = c(t(displace.matrix))
displace.vector

# focal cell is species 1 in resource 2
# surrounded by 4 cells of species 1, 3 of species 2, and 1 of species 3:

cell = 2
resource = 2
n_species = 3
n_resource = 2
neighbors = c(2, 3, 1)


densitySum = 0
densityEach = c()
densityCumul = c()
densityNone = 1
i = 1:3
displace.matrix
displace.vector[(resource - 1) * n_species * n_species + (i - 1) * n_species + cell ]
for(i in 1:n_species){
	densitySum = densitySum + displace.matrix[(resource - 1) * (n_species) + i, cell] * neighbors[i]
	densityEach[i] = displace.matrix[(resource - 1) * (n_species) + i, cell] * neighbors[i]
	}

densityCumul[1] = densityEach[1] / densitySum

for(i in 2:(n_species)){
	densityCumul[i] = densityCumul[i - 1] + densityEach[i] / densitySum
	}

for(i in 0:(n_species  * n_resource - 1)) for(j in 0:(n_species - 1))
		print(paste(n_species * i + j + 1 , " habitat ",floor( i/(n_species )) + 1, " species ", (i)%%n_species + 1, ": species ", j + 1, sep = "" ))
		
		
	
	
	x.i = 1:(n_resource*n_species)
	x.ii = 0:(n_resource*n_species*n_species - 0)
	x.j = 1:n_species
	(x.ii-1)%%(n_species) + 1
	sum(floor(x.ii/(n_species * n_species  +1)))
	floor(x.ii/(n_species * n_species  +1))
	x.ii/(n_species * n_species  +1)
	x.ii%%n_species
	
	cell
	densityEach
	densityCumul

	
	