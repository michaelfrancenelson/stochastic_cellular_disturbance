require(here)

nsp = 5000

c("#000000FF", rainbow(nsp))


write_color_map = function(cols, fname)
{
  cols_out = rbind(
    c(0, 0, 0),
    t(col2rgb(cols))
  )
  write.table(cols_out, fname, sep = " ", row.names = F, col.names = F)
}


write_color_map(rainbow(nsp), here("model_runs", "color_maps", "color_map_5k_sp_rainbow.txt"))


write.table(
  #t(col2rgb(rainbow(nsp + 1))),
  t(col2rgb(c("#000000FF", rainbow(nsp)))),
  here("model_runs", "color_maps", "color_map_5ksp_rainbow.txt"),
  sep = " ", row.names=F, col.names = F)
rainbow(2)

col2rgb(c("#000000", rainbow(2)))
