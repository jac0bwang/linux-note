# TEST Dockerfile

```bash

# build

docker build -t rt_job:1.0.0 .

docker images

# create tag

docker tag rt_job:1.0.0 rt_job:1
docker tag rt_job:1.0.0 rt_job:1.0
docker tag rt_job:1.0.0 rt_job:1.0.0
docker tag rt_job:1.0.0 rt_job:latest

# run docker

docker run -d rt_job
```

# modify Dockerfile

```bash
sudo docker build -t rt_job:1.0.0 .
sudo docker images 
sudo docker tag rt_job:1.0.0 rt_job:1
sudo docker tag rt_job:1.0.0 rt_job:1.0
sudo docker tag rt_job:1.0.0 rt_job:1.0.0
sudo docker tag rt_job:1.0.0 rt_job:latest
sudo docker images
sudo docker run -d rt_job


sudo docker build -t rt_job:1.0.1 .
sudo docker tag rt_job:1.0.1 rt_job:1
sudo docker tag rt_job:1.0.1 rt_job:1.0
sudo docker tag rt_job:1.0.1 rt_job:1.0.1
sudo docker tag rt_job:1.0.1 rt_job:latest
sudo docker run -d rt_job

sudo docker images
sudo docker ps -a # check Exited docker
```
