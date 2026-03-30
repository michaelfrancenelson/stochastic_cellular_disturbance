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


test1_field = matrix(sample(0:3, 20 * 30, replace = T, prob = c(10, 1, 1, 1)), nrow = 20, ncol = 30)
test1_habitat = matrix(c(rep(0, 10 * 30), rep(1, 10 * 30)), nrow = 20, ncol = 30)
test1_field[, ] = 0
test1_field[5, 5] = 1
test1_field[10, 7] = 2
test1_field[15, 15] = 3

test2_habitat = data.frame(matrix(
  c(rep(0, 150 * 400), rep(1, 150*400)),
  nrow = 300, ncol = 400))

test3_habitat = data.frame(matrix(
  sample(c(0, 1), 1e6, replace = T, prob = c(2, 1)),
  nrow = 1000)
)

test3_field = data.frame(matrix(
  sample(c(0:3), 1e6, replace = T, prob = c(10000, 1, 1, 1)),
  nrow = 1000)
)

test4_habitat = data.frame(matrix(
  sample(c(0, 1), 1e6, replace = T, prob = c(2, 1)),
  nrow = 1000)
)

test4_field = data.frame(matrix(
  sample(c(0:10), 1e6, replace = T, prob = c(10000, rep(1, 10))),
  nrow = 1000)
)

nsp = 100
test5_field = data.frame(matrix(
  sample(c(0:nsp), 1e6, replace = T, prob = c(1000, rep(1, nsp))),
  nrow = 1000)
)
test5_habitat = data.frame(matrix(
  sample(c(0, 1), 1e6, replace = T, prob = c(2, 1)),
  nrow = 1000)
)



# Square, 




# Test 7 - 1000 species, 1080p resolution
nsp = 1000
nhab = 1
nrow = 1080
ncol = 1920
test7_field = data.frame(matrix(
  sample(c(0:nsp), nrow * ncol, replace = T, prob = c(10000, rep(1, nsp))),
  nrow = nrow, ncol = ncol)
)
dim(test7_field)
test7_habitat = data.frame(matrix(0, nrow = nrow, ncol = ncol))

t7_disp = make_displacement_probs(nhab, nsp, 0.1, 0.2)
dim(t7_disp)
write.table(make_death_probs(nhab, nsp, 0.1, 0.15), here("test", "test7_death.txt"), sep = " ", row.names = F, col.names = F)
write.table(t7_disp, here("test", "test7_displacement.txt"), sep = " ", row.names = F, col.names = F)
write.table(make_colonize_probs(nhab, nsp, 0.3, 0.5), here("test", "test7_colonize.txt"), sep = " ", row.names = F, col.names = F)

write.table(test7_habitat, here("test", "cfg", "test7_habitat.txt"), sep = " ", row.names = F, col.names = F)
write.table(test7_field, here("test","cfg", "test7_field.txt"), sep = " ", row.names = F, col.names = F)





nsp = 4

test6_field = data.frame(matrix(0, nrow = 1000, ncol = 1000))
test6_habitat = data.frame(matrix(0, nrow = 1000, ncol = 1000))

test6_field[250,250] = 1
test6_field[250,750] = 2
test6_field[750,750] = 3
test6_field[750,250] = 4



write.table(test1_habitat, here("test", "cfg", "test1_habitat.txt"), sep = " ", row.names = F, col.names = F)
write.table(test1_field, here("test", "cfg", "test1_field.txt"), sep = " ", row.names = F, col.names = F)

write.table(test2_habitat, here("test", "test2_habitat.txt"), sep = " ", row.names = F, col.names = F)

write.table(test3_habitat, here("test", "test3_habitat.txt"), sep = " ", row.names = F, col.names = F)
write.table(test3_field, here("test", "test3_field.txt"), sep = " ", row.names = F, col.names = F)

write.table(test4_habitat, here("test", "test4_habitat.txt"), sep = " ", row.names = F, col.names = F)
write.table(test4_field, here("test", "test4_field.txt"), sep = " ", row.names = F, col.names = F)

write.table(test5_habitat, here("test", "test5_habitat.txt"), sep = " ", row.names = F, col.names = F)
write.table(test5_field, here("test", "test5_field.txt"), sep = " ", row.names = F, col.names = F)

write.table(test6_habitat, here("test", "test6_habitat.txt"), sep = " ", row.names = F, col.names = F)
write.table(test6_field, here("test", "test6_field.txt"), sep = " ", row.names = F, col.names = F)


# Test 4 - 10 species
write.table(make_death_probs(2, 10, 0.01, 0.02), here("test", "test4_death.txt"), sep = " ", row.names = F, col.names = F)
write.table(make_displacement_probs(2, 10, 0.1, 0.2), here("test", "test4_displacement.txt"), sep = " ", row.names = F, col.names = F)
write.table(make_colonize_probs(2, 10, 0.1, 0.5), here("test", "test4_colonize.txt"), sep = " ", row.names = F, col.names = F)

# test 5 - 100 species
write.table(make_death_probs(2, 100, 0.01, 0.02), here("test", "test5_death.txt"), sep = " ", row.names = F, col.names = F)
write.table(make_displacement_probs(2, 100, 0.1, 0.2), here("test", "test5_displacement.txt"), sep = " ", row.names = F, col.names = F)
write.table(make_colonize_probs(2, 100, 0.1, 0.5), here("test", "test5_colonize.txt"), sep = " ", row.names = F, col.names = F)

# test 6 - 4 species
write.table(make_death_probs(1, 4, 0.4, 0.4), here("test", "test6_death.txt"), sep = " ", row.names = F, col.names = F)
write.table(make_displacement_probs(1, 4, 0.2, 0.2), here("test", "test6_displacement.txt"), sep = " ", row.names = F, col.names = F)
write.table(make_colonize_probs(1, 4, 0.5, 0.5), here("test", "test6_colonize.txt"), sep = " ", row.names = F, col.names = F)



#### - Color Maps
hcl.colors(12, "ylOrRd", rev = T)


write.table(
  t(col2rgb(hcl.colors(101, "ylOrRd", rev = T))),
  here("test", "color_maps", "color_map_100sp.txt"),
  sep = " ", row.names=F, col.names = F)
write.table(
  t(col2rgb(hcl.colors(5, "ylOrRd", rev = T))),
  here("test", "color_maps", "color_map_4sp.txt"),
  sep = " ", row.names=F, col.names = F)

nsp = 1000
write.table(
  t(col2rgb(hcl.colors(nsp + 1, "ylOrRd", rev = T))),
  #t(col2rgb(terrain.colors(1001))),
  #t(col2rgb(heat.colors(1001))),
  here("test", "color_maps", "color_map_1ksp_hcl.txt"),
  sep = " ", row.names=F, col.names = F)



pois_test = c(4, 4, 2, 2, 3, 1, 0, 4, 1, 4, 0, 1, 1, 1, 2, 6, 5, 3, 3, 1, 2, 1, 2, 0, 1, 3, 1, 3, 2, 1, 3, 0, 2, 2, 0, 4, 2, 3, 4, 2, 2, 4, 3, 2, 2, 3, 3, 2, 3, 0, 3, 1, 2, 3, 6, 1, 1, 1, 2, 4, 1, 3, 3, 2, 2, 1, 4, 3, 3, 1, 2, 2, 1, 4, 3, 4, 5, 0, 2, 2, 3, 2, 4, 2, 1, 1, 1, 0, 3, 2, 2, 0, 0, 5, 2, 4, 2, 3, 4, 7, 1, 5, 6, 2, 3, 4, 1, 2, 3, 1, 2, 3, 3, 1, 2, 2, 3, 1, 4, 1, 3, 3, 4, 2, 3, 7, 0, 2, 3, 5, 1, 4, 0, 1, 1, 6, 4, 4, 6, 4, 1, 1, 2, 1, 0, 2, 0, 1, 1, 1, 3, 2, 2, 2, 3, 4, 2, 1, 1, 3, 3, 1, 1, 2, 0, 0, 5, 2, 2, 3, 1, 1, 3, 7, 2, 2, 2, 5, 5, 2, 1, 2, 1, 4, 3, 1, 6, 3, 1, 3, 3, 4, 3, 4, 4, 2, 6, 8, 2, 2, 3, 1, 5, 2, 4, 3, 2, 4, 1, 2, 4, 2, 3, 2, 2, 0, 2, 2, 2, 1, 1, 2, 1, 5, 1, 4, 5, 2, 2, 1, 1, 2, 2, 3, 4, 1, 0, 1, 2, 3, 5, 0, 7, 2, 2, 3, 1, 3, 2, 2, 1, 5, 6, 3, 2, 3, 3, 3, 3, 3, 2, 1, 2, 3, 1, 0, 3, 7, 7, 1, 2, 4, 3, 0, 1, 3, 1, 5, 3, 2, 1, 2, 2, 0, 1, 2, 3, 4, 0, 1, 4, 2, 2, 1, 2, 5, 3, 5, 3, 3, 1, 2, 4, 4, 5, 3, 4, 1, 0, 4, 0, 2, 2, 2, 4, 2, 2, 5, 4, 3, 2, 3, 1, 3, 3, 2, 1, 2, 2, 3, 2, 5, 1, 4, 3, 1, 7, 0, 1, 2, 1, 4, 3, 3, 0, 1, 1, 2, 0, 2, 3, 2, 1, 2, 6, 2, 3, 4, 2, 1, 2, 3, 1, 1, 3, 2, 3, 6, 1, 2, 3, 2, 3, 0, 3, 2, 2, 3, 3, 1, 3, 3, 3, 2, 3, 2, 2, 2, 3, 4, 0, 3, 4, 8, 5, 3, 1, 4, 1, 1, 5, 2, 2, 1, 2, 4, 2, 0, 0, 0, 1, 5, 0, 2, 2, 0, 2, 3, 2, 3, 0, 2, 2, 3, 2, 0, 3, 4, 3, 1, 4, 2, 2, 0, 5, 4, 1, 3, 2, 3, 3, 3, 4, 3, 3, 5, 1, 2, 3, 1, 7, 2, 1, 2, 1, 1, 5, 2, 0, 2, 0, 3, 3, 2, 1, 3, 2, 2, 0, 4, 0, 6, 4, 2, 5, 2, 6, 3, 0, 4, 3, 4, 2, 0, 5, 3, 0, 2, 1, 4, 2, 5, 1, 2, 2, 2, 1, 1, 3, 1, 0, 1, 4, 2, 3, 0, 1, 1, 2, 6, 2, 4, 6, 4, 3, 6, 0, 4, 3, 2, 4, 3, 0, 1, 5, 2, 4, 2, 2, 1, 2, 2, 3, 3, 4, 7, 1, 0, 3, 2, 1, 1, 1, 2, 3, 4, 2, 3, 3, 2, 0, 1, 3, 1, 1, 4, 2, 2, 3, 4, 5, 3, 7, 2, 3, 2, 2, 3, 6, 2, 4, 0, 2, 3, 0, 2, 2, 2, 1, 4, 4, 1, 1, 2, 4, 0, 1, 3, 2, 2, 1, 0, 1, 3, 2, 0, 2, 2, 1, 2, 2, 7, 2, 2, 5, 1, 2, 5, 2, 0, 2, 2, 2, 1, 2, 1, 1, 2, 3, 3, 5, 1, 3, 3, 6, 0, 0, 4, 0, 1, 2, 2, 5, 4, 2, 2, 4, 7, 2, 3, 5, 1, 2, 5, 3, 1, 2, 4, 1, 3, 5, 2, 3, 3, 1, 3, 2, 5, 4, 0, 3, 2, 1, 1, 1, 3, 0, 2, 2, 3, 2, 0, 2, 4, 1, 1, 3, 2, 3, 1, 1, 2, 1, 4, 1, 6, 3, 3, 5, 3, 4, 1, 2, 5, 3, 4, 1, 1, 1, 4, 1, 2, 4, 5, 1, 4, 1, 2, 3, 3, 5, 3, 2, 2, 1, 2, 1, 1, 2, 2, 3, 6, 2, 3, 8, 1, 2, 1, 3, 0, 0, 2, 3, 2, 1, 3, 1, 4, 2, 6, 2, 2, 1, 1, 1, 4, 2, 3, 3, 2, 3, 1, 3, 3, 6, 2, 2, 3, 3, 5, 3, 7, 2, 3, 3, 3, 3, 2, 6, 3, 3, 1, 1, 3, 3, 4, 4, 3, 1, 4, 1, 2, 3, 2, 3, 2, 5, 1, 3, 2, 1, 2, 1, 5, 4, 0, 3, 3, 3, 4, 2, 4, 1, 1, 3, 3, 4, 5, 5, 2, 2, 3, 0, 1, 3, 5, 4, 3, 4, 1, 3, 2, 1, 2, 4, 3, 0, 3, 0, 1, 1, 1, 2, 0, 5, 1, 1, 1, 1, 3, 3, 3, 1, 2, 1, 3, 1, 0, 4, 1, 1, 4, 1, 1, 2, 3, 5, 2, 1, 3, 2, 2, 3, 3, 4, 4, 4, 4, 5, 3, 1, 1, 1, 1, 2, 3, 7, 1, 3, 2, 2, 1, 1, 6, 0, 2, 1, 1, 1, 1, 1, 0, 2, 4, 6, 2, 3, 2, 3, 3, 3, 4, 7, 3, 4, 4, 3, 2, 4, 5, 1, 0, 2, 1, 4, 2, 5, 3, 4, 3, 4, 7, 4, 4, 5, 1, 1, 5, 2, 3, 1, 3, 4, 0, 1, 2, 2, 5, 3, 4, 0, 3, 0, 1, 4, 0, 5, 1, 3, 6, 1, 4, 4, 2, 3, 3, 2, 1, 2, 3, 4, 2, 2, 3, 3, 2, 3, 2, 2, 6, 4, 1, 4, 4, 2, 2, 5, 3, 2, 5, 3, 1, 2, 1, 1, 6, 3, 6, 5, 0, 0, 8, 2, 2, 4, 2, 2, 2, 3, 3, 3)

var(pois_test)
mean(pois_test)

hist(pois_test)


