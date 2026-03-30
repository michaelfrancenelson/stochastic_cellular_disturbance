file.in.1 <- "/Documents/Research/Phalaris.spatial.models/model.2/runs/test/out.equal/census.1.txt"

file.in.2 <- "/Documents/Research/Phalaris.spatial.models/model.2/runs/test/out.equal2/census.1.txt"

file.in.3 <- "/Documents/Research/Phalaris.spatial.models/model.2/runs/test/out.equal3/census.1.txt"

file.in.4 <- "/Documents/Research/Phalaris.spatial.models/model.2/runs/test/out.equal4/census.1.txt"



dat.1 <- read.table(file.in.1, header = T)
dat.2 <- read.table(file.in.2, header = T)
dat.3 <- read.table(file.in.3, header = T)
dat.4 <- read.table(file.in.4, header = T)


head(dat)

max(dat)

par(mfrow = c(2,2))

plot(dat.1$Time, dat.1$Pop_1, type = "l", ylim = c(0, max(dat.1)), col = 1, xlab = "Time", ylab = "Population", main = "Trial 1")
points(dat.1$Time, dat.1$Pop_2, type = "l", col = 2)

plot(dat.2$Time, dat.2$Pop_1, type = "l", ylim = c(0, max(dat.2)), col = 1, xlab = "Time", ylab = "Population", main = "Trial 2")
points(dat.2$Time, dat.2$Pop_2, type = "l", col = 2)

plot(dat.3$Time, dat.3$Pop_1, type = "l", ylim = c(0, max(dat.3)), col = 1, xlab = "Time", ylab = "Population", main = "Trial 3")
points(dat.3$Time, dat.3$Pop_2, type = "l", col = 2)

plot(dat.4$Time, dat.4$Pop_1, type = "l", ylim = c(0, max(dat.4)), col = 1, xlab = "Time", ylab = "Population", main = "Trial 4")
points(dat.4$Time, dat.4$Pop_2, type = "l", col = 2)

#legend(x=0, y = max(dat), legend = list("species 1: competitor", "species 2: colonizer"), box.col = 1, col = c(1, 2), lty = 1, cex = 0.75)
