# Terraform 

<br/>

## Important Parts of tf files

<br/>

### Terraform Block
The terraform block contains Terraform settings, including the required providers Terraform will use to provision your infrastructure. For each provider, the source attribute defines an optional hostname, a namespace, and the provider type. Terraform installs providers from the Terraform Registry by default.

You can also define a version constraint for each provider in the required_providers block. The version attribute is optional, but we recommend using it to enforce the provider version. Without it, Terraform will always use the latest version of the provider, which may introduce breaking changes.

This is an example

```
terraform {
  required_providers {
    docker = {
      source  = "kreuzwerker/docker"
      version = "~> 3.0.1"
    }
  }
}
```

<br/>

### Providers
The provider block configures the specified provider, in this case docker. A provider is a plugin that Terraform uses to create and manage your resources. You can define multiple provider blocks in a Terraform configuration to manage resources from different providers.

This is an example

```
provider "docker" {
  host = "npipe:////.//pipe//docker_engine"
}
```

<br/>

### Resource
Use resource blocks to define components of your infrastructure. A resource might be a physical component such as a server, or it can be a logical resource such as a Docker Container.

Resource blocks have two strings before the block: the resource type and the resource name. In the example below, the resource type is docker_image and the name is nginx. The prefix of the type maps to the name of the provider. In the example configuration, Terraform manages the docker_image resource with the docker provider. Together, the resource type and resource name form a unique ID for the resource. For example, the ID for your network is docker_image.nginx.

This is an example.

```
resource "docker_image" "nginx" {
  name         = "nginx"
  keep_locally = false
}
```

<br/>

## Are they absolutely necessary when running a tf file?

<br/>

#### Terraform Block 
* Not always required but recommended.
* Used to define required providers, backend configurations, and settings.
* If omitted, Terraform will still work, but you'll need to manually specify providers in terraform init.

<br/>

#### Provider Block 
* Required if the configuration needs a provider (e.g., docker, aws, azurerm).
* Specifies the service Terraform will interact with.
* Terraform won't work without it if a resource depends on a provider.

<br/>

#### Resource Block 
* Required to actually create infrastructure.
* Defines what Terraform should build (e.g., containers, VMs, networks).

<br/>

#### In conclusion

* **Terraform Block** is not required but is highly recommended.

* **Provider Block** is required if resources use a provider.

* **Resource Block** is required if you want Terraform to create something.










