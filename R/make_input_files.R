require(here)
require(data.tab.e)


# New parameterization ----



# 3 species ----

nrow = 1000
ncol = 1000
fwrite(
  matrix(sample(0:3, nrow * ncol, replace = T, prob = c(100, 1, 1, 1)), nrow = nrow, ncol = ncol),
  here("test", "input", "3sp_field.txt"),
  sep = " ", col.names = F)
fwrite(
  matrix(sample(0:1, nrow * ncol, replace = T), nrow = nrow, ncol = ncol),
  here("test", "input", "3sp_habitat.txt"),
  sep = " ", col.names = F)



# 100 species

nsp = 100
nhab = 2

energy = 0.5





params_100sp = data.frame(
  species = rep(0:nsp, nhab),
  habitat = rep(0:(nhab - 1), each = nsp + 1), 
  death_prob = 0, 
  displace_prob = c(0, runif(nsp, min = 0, max = energy), 0, runif(nsp, min = 0, max = energy)))

params_100sp$colonize_prob = with(params_100sp, energy - displace_prob)
params_100sp$colonize_prob[c(1, nsp + 2)] = 0

write.csv(params_100sp, here("test", "cfg", "params_100sp_1.csv"), row.names = F)
params_100sp

nrow = 1000; ncol = 1000
fwrite(
  matrix(sample(0:100, nrow * ncol, replace = T, prob = c(100, rep(1, nsp))), nrow = nrow, ncol = ncol),
  here("test", "input", "100sp_field.txt"),
  sep = " ", col.names = F)
fwrite(
  matrix(
    rep(0, nrow * ncol * 0.5), rep(1, nrow * ncol * 0.5),
    #sample(0:1, nrow * ncol, replace = T),
    nrow = nrow, ncol = ncol),
  here("test", "input", "100sp_habitat.txt"),
  sep = " ", col.names = F)

fwrite(
  matrix(
    rep(0, nrow * ncol * 0.5), rep(1, nrow * ncol * 0.5),
    #sample(0:1, nrow * ncol, replace = T),
    nrow = nrow, ncol = ncol),
  here("test", "input", "1k_1k_2_habitat.txt"),
  sep = " ", col.names = F)
fwrite(
  matrix(
    rep(0, nrow * ncol * 0.5), rep(0, nrow * ncol * 0.5),
    #sample(0:1, nrow * ncol, replace = T),
    nrow = nrow, ncol = ncol),
  here("test", "input", "1k_1k_1_habitat.txt"),
  sep = " ", col.names = F)


# Tradeoff Species ----

nsp = 100
nhab = 1

min_comp = 0.05
max_comp = 0.15

comp_100 = c(0, runif(nsp, min_comp, max_comp))
col_100 = c(0, 0.3 - comp_100[-1])

params_100sp_tradeoff = data.frame(
  species = 0:nsp,
  habitat = rep(0, nsp + 1), 
  death_prob = 0, 
  displace_prob = comp_100,
  colonize_prob = col_100)
write.csv(params_100sp_tradeoff, here("test", "cfg", "params_100sp_tradeoff.csv"), row.names = F)



# 5000 species

nsp = 5000
nhab = 1

min_comp = 0.05
max_comp = 0.15

comp_5k = c(0, runif(nsp, min_comp, max_comp))
col_5k = c(0, 0.5 - 2 * comp_5k[-1])

params_5k_sp_tradeoff = data.frame(
  species = 0:nsp,
  habitat = rep(0, nsp + 1), 
  death_prob = 0, 
  displace_prob = comp_5k,
  colonize_prob = col_5k)
write.csv(params_5k_sp_tradeoff, here("test", "cfg", "params_5k_sp_tradeoff.csv"), row.names = F)



set.seed(1)
fwrite(
  matrix(sample(0:5000, 1e6, replace = T, prob = c(1000, rep(1, 5000))), nrow = 1000, ncol = 1000),
  here("test", "input", "5k_sp_field_1.txt"),
       sep = " ", col.names = F)




















make_death_probs = function(nhab, nsp, min, max)
{
  mat = matrix(runif(
    nhab * (nsp + 1), min = min, max = max),
    nrow = nsp + 1)
  mat[1, ] = 0
  return(mat)
}
make_displacement_probs = function(nhab, nsp, min, max)
{
  mat = matrix(runif(
    (nsp + 1)*(nsp + 1) * nhab,min = min, max = max),
    nrow = nhab * (nsp + 1))
  mat[1, ] = 0
  mat[, 1] = 0
  return(mat)
}
make_colonize_probs = function(nhab, nsp, min, max)
{
  mat = matrix(
    runif(
      nhab * (nsp + 1), min = min, max = max),
    nrow = nsp + 1)
  mat[1, ] = 0
  return(mat)
}










set.seed(1)


# 5K species ----
set.seed(1)
field_5k_species =  matrix(sample(1:5000, 1e6, replace = T), nrow = 1000, ncol = 1000)
habitat_5k_species =  matrix(0, nrow = 1000, ncol = 1000)

write.table(habitat_5k_species, here("model_runs", "input", "5k_species_habitat.txt"), sep = " ", row.names = F, col.names = F)
write.table(field_5k_species, here("model_runs", "input", "5k_species_field1.txt"), sep = " ", row.names = F, col.names = F)

set.seed(1)
write.table(make_death_probs(1, 5000, 0.01, 0.02), here("model_runs", "input", "5k_species_death1.txt"), sep = " ", row.names = F, col.names = F)
write.table(make_displacement_probs(1, 5000, 0.1, 0.2), here("model_runs", "input", "5k_species_displace1.txt"), sep = " ", row.names = F, col.names = F)
write.table(make_colonize_probs(1, 5000, 0.1, 0.2), here("model_runs", "input", "5k_species_colonize1.txt"), sep = " ", row.names = F, col.names = F)

write.table(make_death_probs(1, 5000, 0.03, 0.05), here("model_runs", "input", "5k_species_death2.txt"), sep = " ", row.names = F, col.names = F)
write.table(make_displacement_probs(1, 5000, 0.2, 0.4), here("model_runs", "input", "5k_species_displace2.txt"), sep = " ", row.names = F, col.names = F)
write.table(make_colonize_probs(1, 5000, 0.2, 0.4), here("model_runs", "input", "5k_species_colonize2.txt"), sep = " ", row.names = F, col.names = F)

write.table(
  t(col2rgb(rainbow(5001))),
  here("model_runs", "input", "5k_sp_color_map_rainbow.txt"),
  sep = " ", row.names=F, col.names = F)



# 100 species ----
nsp = 100
nhab = 1

set.seed(1)
field_5k_species =  matrix(sample(1:nsp, 1e6, replace = T), nrow = 1000, ncol = 1000)
habitat_5k_species =  matrix(0, nrow = 1000, ncol = 1000)

write.table(habitat_5k_species, here("model_runs", "input", "100_species_habitat.txt"), sep = " ", row.names = F, col.names = F)
write.table(field_5k_species, here("model_runs", "input", "100_species_field1.txt"), sep = " ", row.names = F, col.names = F)

# test 5 - 100 species
set.seed(1)
write.table(make_death_probs(nhab, nsp, 0.01, 0.02), here("model_runs", "input", "100_species_death1.txt"), sep = " ", row.names = F, col.names = F)
write.table(make_displacement_probs(nhab, nsp, 0.1, 0.2), here("model_runs", "input", "100_species_displace1.txt"), sep = " ", row.names = F, col.names = F)
write.table(make_colonize_probs(nhab, nsp, 0.1, 0.2), here("model_runs", "input", "100_species_colonize1.txt"), sep = " ", row.names = F, col.names = F)

write.table(
  t(col2rgb(rainbow(nsp + 1))),
  here("model_runs", "input", "100_sp_color_map_rainbow.txt"),
  sep = " ", row.names=F, col.names = F)




# 1000 species ----
set.seed(1)
nsp = 1000
nhab = 1

set.seed(1)
field_1k_species =  matrix(sample(1:nsp, 1e6, replace = T), nrow = 1000, ncol = 1000)
habitat_1k_species =  matrix(0, nrow = 1000, ncol = 1000)

write.table(habitat_1k_species, here("model_runs", "input", "1k_species_habitat.txt"), sep = " ", row.names = F, col.names = F)
write.table(field_1k_species, here("model_runs", "input", "1k_species_field1.txt"), sep = " ", row.names = F, col.names = F)

# test 5 - 100 species
set.seed(1)
write.table(make_death_probs(nhab, nsp, 0.01, 0.02), here("model_runs", "input", "1k_species_death1.txt"), sep = " ", row.names = F, col.names = F)
write.table(make_displacement_probs(nhab, nsp, 0.1, 0.2), here("model_runs", "input", "1k_species_displace1.txt"), sep = " ", row.names = F, col.names = F)
write.table(make_colonize_probs(nhab, nsp, 0.1, 0.2), here("model_runs", "input", "1k_species_colonize1.txt"), sep = " ", row.names = F, col.names = F)

write.table(
  t(col2rgb(rainbow(nsp + 1))),
  here("model_runs", "input", "1k_sp_color_map_rainbow.txt"),
  sep = " ", row.names=F, col.names = F)






