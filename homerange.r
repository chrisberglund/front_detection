library(sf)
library(raster)
file.names <- dir("freq/", pattern =".csv")
for(i in 1:length(file.names)){
  freq <- read.csv(paste("freq/",file.names[i], sep=""))
  coordinates(freq) <- ~Longitude+Latitude
  proj4string(freq) <- "+proj=longlat"
  spfreq <- spTransform(freq, CRS("+proj=sinu +lon_0=0 +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"))
  month <- substr(file.names[i], start=6,stop=7)
  aoi_boundary_HARV <- st_read(paste("kud/",month,"KUD99.shp", sep=""))
  homerange <- sf::as_Spatial(st_geometry(aoi_boundary_HARV[[1,2]]), )
  proj4string(homerange) <- "+proj=longlat"
  homerange = spTransform(homerange, CRS(proj4string(spfreq)))
  homerange.fronts <- spfreq[!is.na(over(spfreq,homerange)),]
  df <- data.frame(homerange.fronts)
  write.csv(df, file = paste("homerange/", file.names[i], sep=""))
  print(file.names[i])
 }