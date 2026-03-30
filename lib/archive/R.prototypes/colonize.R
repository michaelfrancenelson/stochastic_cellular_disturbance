# Prototype for the colonize() function:

# example with 3 species:
n.species = 3
# colonization probabilities
c.probs = c(0.5, 0.5, 0.9); c.probs
# number of each neighbor
n.counts = c(3, 5, 0); n.counts
c.probs = c(0.75, 0.25, 0.1); c.probs
n.counts = c(1, 3, 0); n.counts

i = 1
i = 2



# calculate the density of no colonization
density.none = prod((1 - c.probs) ^ n.counts)
(1 - c.probs) ^ n.counts
density.none

# the density remaining for the others:
1 - density.none

# total colonization density for each species = count of each neghbor multiplied by its probability
t.density =  c.probs * n.counts ; t.density
sum(t.density)

# normalize so they add to (1 - density.none):
t.density.n1 = (1 - density.none) * t.density / sum(c.probs * n.counts)
t.density.n1

# probability of none + sum of other probs = 1
sum(t.density.n1) + density.none

# make a table:
cum.prob =cumsum(c(density.none, t.density.n1))
cbind(neighbor.id = 0:length(n.counts), cum.prob)

# which one wins?
uniform.random = runif(1); uniform.random
species.win = 0

# lottery:
for(i in 1:length(cum.prob)){
	if(uniform.random <= cum.prob[i]){
		species.win = i - 1
		break
		}
	}
	

sum(exp(-5) * 5^(2:1) / prod(2:1))
dbinom(2, 20, .25)
(exp(-5) * 5^0) + (exp(-5) * 5^(1) / prod(1)) + (exp(-5) * 5^(2) / prod(2:1))
