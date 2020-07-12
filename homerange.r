library(sf)
library(raster)
file.names <- dir("sst_freq/", pattern =".csv")
for(i in 1:length(file.names)){
  freq <- read.csv(paste("sst_freq/",file.names[i], sep=""))
  coordinates(freq) <- ~Longitude+Latitude
  proj4string(freq) <- "+proj=longlat"
  #month_freq <- freq[strtoi(freq$Month) == j,]
  spfreq <- spTransform(freq, CRS("+proj=sinu +lon_0=0 +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"))
  aoi_boundary_HARV <- st_read(paste("kud/", 1,"KUD95.shp", sep=""))
  print(aoi_boundary_HARV)
  homerange <- sf::as_Spatial(st_geometry(aoi_boundary_HARV), )
  proj4string(homerange) <- "+proj=longlat"
  homerange = spTransform(homerange, CRS(proj4string(spfreq)))
  homerange.fronts <- spfreq[!is.na(over(spfreq,homerange)),]
  df <- data.frame(homerange.fronts)
  write.csv(df, file = paste("sst_homerange/",  file.names[i], sep=""))
 }