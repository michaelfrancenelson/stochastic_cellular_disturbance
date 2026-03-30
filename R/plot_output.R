require(here)
require(data.table)
require(animation)

nRow = 20
nCol = 30

plot_grid = function(dat, step, nRow = 20, nCol = 30, nSpecies = 3, col = c(1, hcl.colors(12, "ylOrRd", rev = T)))
{
  par(mar = c(0, 0, 0, 0), oma = c(0, 0, 0, 0))
  image(
    1:nRow,
    1:nCol,
    dat[(nRow * step + 1):(nRow * (step + 1)), ],
    zlim= c(0, nSpecies),
    col = col)
}
video_grid = function(
    dat, nRow, nCol, nSteps,
    col,
    interval = 0.2,
    videoName = "vid1.mp4", 
    otherOpts = "-pix_fmt yuv420p -b 300k")
{ 
  saveVideo({
    par(mar = c(0, 0, 0, 0), oma = c(0, 0, 0, 0))
    ani.options(
      interval = interval, nmax = nSteps,
      ani.width = nCol, ani.height = nRow)
    for(i in 1:nSteps)
    {
      plot_grid(dat, i, nRow, nCol, col = col)
      print(sprintf("Step %d of %d.", i, nSteps))
    }
  }, video.name = videoName, other.opts = otherOpts)
}


dat_test1 = (as.matrix(fread(here("test", "out", "test1_field_out.txt"), header = F)))
plot_grid(dat_test1, 99)



dat_test2 = (as.matrix(fread(here("test", "out", "test2_field_out.txt"), header = F)))
plot_grid(dat_test2, 200, 300, 400)
dev.off()

dat_test3 = (as.matrix(fread(here("test", "out", "test3_field_out.txt"), header = F)))
plot_grid(dat_test3, 100, 1000, 1000)

dat_test4 = (as.matrix(fread(here("test", "out", "test4_field_out.txt"), header = F)))
plot_grid(dat_test4, 100, 1000, 1000, nSpecies = 10)

census_test4 = fread(here("test", "out", "test4_census_out.txt"))
matplot(census_test4[,-1], type = "l", lty = 1)

census_test5 = fread(here("test", "out", "test5_census_out.txt"))
matplot(census_test5[,-1], type = "l", lty = 1)







sum(dat_test4[19000:2000, 1:1000] == 10)


tail(dat_10sp)

sum(dat_10sp)

dev.off()

png(here("test", "out", "img", "test_10sp.png"), height = 1000, width = 1000)


for(i in 101:1000)
{
  print(i)
  png(
    here("test", "out", "img", sprintf("step%04d.png", i)),
    height = 1000, width = 1000)
  plot_grid(dat_10sp, i, 1000, 1000, 10)
  dev.off()
}



# ffmpeg -i step%04d.png -c:v libx264 -vf fps=25 -pix_fmt yuv420p out.mp4

dev.off()


plot_grid(dat, 700, 1000, 1000, 3)

plot_grid(dat, 10)

video_grid(dat, 20, 30, 200)

video_grid(
  dat1k, 1000, 1000, 1000,
  videoName = here("test", "out", "vid1k2.mp4"),
  otherOpts = "-vf scale=1000:1000 -c:v libx264 -crf 23")

video_grid(
  dat1k, 1000, 1000, 20,
  col = terrain.colors(4),
  interval = 0.15,
  videoName = here("test", "out", "vid1k2.mp4"),
  otherOpts = "-vf scale=1000:1000 -c:v libx264 -crf 23")


video_grid(
  dat_10sp, 1000, 1000, 20,
  col = terrain.colors(11),
  interval = 0.15,
  videoName = here("test", "out", "test_10sp.mp4"),
  otherOpts = "-vf scale=1000:1000 -c:v libx264 -crf 23")



dat2 = as.matrix(fread(here("test", "test2_out.txt"), header = F))
plot_grid(dat2, 600, 300, 400)

video_grid(dat2, 300, 400, 1000, "big_grid.mp4",)



