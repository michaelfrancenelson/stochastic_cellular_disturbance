require(here)

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

# test 5 - 100 species
set.seed(1)
write.table(make_death_probs(1, 5000, 0.01, 0.02), here("model_runs", "input", "5k_species_death1.txt"), sep = " ", row.names = F, col.names = F)
write.table(make_displacement_probs(1, 5000, 0.1, 0.2), here("model_runs", "input", "5k_species_displace1.txt"), sep = " ", row.names = F, col.names = F)
write.table(make_colonize_probs(1, 5000, 0.1, 0.2), here("model_runs", "input", "5k_species_colonize1.txt"), sep = " ", row.names = F, col.names = F)

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



