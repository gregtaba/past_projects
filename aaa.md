# Terraform-Docker 

<br/>

## Building Images and Containers
Terraform has the ability to create multiple Images and Containers in Docker.All you need to do is configure your tf file accordingly.

<br/>

For example this below piece of code builds the images and containers for MySQL and Nginx.

```

terraform {
  required_providers {
    docker = {
      source  = "kreuzwerker/docker"
      version = "~> 3.0.1"
    }
  }
}

provider "docker" {
  host = "npipe:////.//pipe//docker_engine"
}

# Create a network
resource "docker_network" "app_network" {
  name = "app_network"
}

# Pull and use Nginx image
resource "docker_image" "nginx" {
  name         = "nginx"
  keep_locally = false
}

resource "docker_container" "nginx" {
  image = docker_image.nginx.image_id
  name  = "nginx-server"

  ports {
    internal = 80
    external = 8080
  }

  networks_advanced {
    name = docker_network.app_network.name
  }


}

# Pull and use MySQL image
resource "docker_image" "mysql" {
  name         = "mysql"
  keep_locally = false
}

resource "docker_container" "mysql" {
  image = docker_image.mysql.image_id
  name  = "mysql-db"

  env = [
    "MYSQL_ROOT_PASSWORD=rootpass",
    "MYSQL_DATABASE=mydb"
  ]

  ports {
    internal = 3306
    external = 3306
  }

  networks_advanced {
    name = docker_network.app_network.name
  }


}



```
<br/>

#### Important notes about the keep_locally variable
The variable **keep_locally** basically tells terraform whether to delete the images along the containers when the command **terraform destroy** will be used .

* If **keep_locally is false** then the command terraform destroy will delete containers and images created by terraform.

* If **keep_locally is true** then the command terraform destroy will delete containers created by terraform and keep the images.

<br/>

#### Important note about the command terraform destroy
When **terraform builds more than 2 images with keep_locally as false** it has been observed that **the command terraform destroy only deletes some of the images and then returns an error**.However if you run a second terraform destroy then the remaining images get deleted as well. In any case all the containers get deleted despite the number that was created.

<br/>

#### Important note about the structure of the code
If you want to ,it is possible to create multiple tf files when building containers in docker,however you need to keep them in the same directory.Below is an example of the original example now divided in two tf files.

<br/>

###### First tf file

```

terraform {
  required_providers {
    docker = {
      source  = "kreuzwerker/docker"
      version = "~> 3.0.1"
    }
  }
}

provider "docker" {
  host = "npipe:////.//pipe//docker_engine"
}

# Create a network
resource "docker_network" "app_network" {
  name = "app_network"
}

# Pull and use Nginx image
resource "docker_image" "nginx" {
  name         = "nginx"
  keep_locally = false
}

resource "docker_container" "nginx" {
  image = docker_image.nginx.image_id
  name  = "nginx-server"

  ports {
    internal = 80
    external = 8080
  }

  networks_advanced {
    name = docker_network.app_network.name
  }


}



```
###### Second tf file
```


# Pull and use MySQL image
resource "docker_image" "mysql" {
  name         = "mysql"
  keep_locally = false
}

resource "docker_container" "mysql" {
  image = docker_image.mysql.image_id
  name  = "mysql-db"

  env = [
    "MYSQL_ROOT_PASSWORD=rootpass",
    "MYSQL_DATABASE=mydb"
  ]

  ports {
    internal = 3306
    external = 3306
  }

  networks_advanced {
    name = docker_network.app_network.name
  }


}



```

**Notice that the required_providers and provider is mentioned only once**.That is because terraform basically merges and runs all of the tf files in the directory simultaneously. Therefore if we declare required_providers and provider twice we will get an error.

<br/>

## Syntax Features

<br/>

#### Environment Variables
As we can see in the code above terraform files also allow us to set environment veriables in this syntax.

```
  env = [
    "MYSQL_ROOT_PASSWORD=rootpass",
    "MYSQL_DATABASE=mydb"
  ]

```

<br/>

#### Tag
In tf files if you don't specify a tag then the "latest" one will be used as it is the default one. In the example below since no tag is specified terraform uses the "latest" tag. 

```
resource "docker_image" "nginx" {
  name         = "nginx"
  keep_locally = false
}

```

If you want to specify the version you want to use you can do it in this way.

```
resource "docker_image" "nginx" {
  name         = "nginx:1.21"  # specifying version 1.21
  keep_locally = false
}
```
<br/>

#### Ports for containers
As we can see in the initial code terraform files also allow us to set ports for our containers with this syntax

```
  ports {
    internal = 80
    external = 8080
  }

```

<br/>

#### Network Creation
As we can see in the initial code terraform files also allow us to create networks in order for our containers to communicate with this syntax. 

```
resource "docker_network" "app_network" {
  name = "app_network"
}
```

In order for the container to access the network it needs this piece of code 

```
  networks_advanced {
    name = docker_network.app_network.name
  }
```

<br/>

#### Volumes


Terraform files also allow us to create volumes for our containers so that their data is saved after the container is shut down, using this syntax.

```
  volumes {
    "/usr/share/nginx/html" = "/path/on/host"  # Example volume mount for Nginx
  }
```
the paths in this piece of code are examples and not actually correct.


<br/>

## Changes to the tf file

If changes have been made to the tf file after its first terraform apply command in order for the user to apply the changes they need to use the command terraform apply again (preferably after using terraform plan and terraform validate to check the changes that will be made as well as the validity of the tf file before running it).









