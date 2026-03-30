file.in <- "/Documents/Research/Phalaris.spatial.models/model.2/runs/test/out.equal2/census.1.txt"

dat <- read.table(file.in, header = T)
head(dat)

max(dat)



plot(dat$Time, dat$Pop_1, type = "l", ylim = c(0, max(dat)), col = 1, xlab = "Time", ylab = "Population")
points(dat$Time, dat$Pop_2, type = "l", col = 2)

#legend(x=0, y = max(dat), legend = list("species 1: competitor", "species 2: colonizer"), box.col = 1, col = c(1, 2), lty = 1, cex = 0.75)

title("disturbance: 7 patches of size 50 each generation")