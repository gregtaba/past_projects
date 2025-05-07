# Terraform 

<br/>

## Basic Commands

```

terraform init

terraform fmt

terraform plan 

terraform validate

terraform apply

terraform show

terraform destroy

terraform stage
```

<br/>

## terraform init
This command initializes a new or existing Terraform working directory , it is necessary before being able to run your tf file.

<br/>

## terraform fmt
The terraform fmt command automatically updates configurations in the current directory for readability and
consistency.

<br/>

## terraform plan
Creates an execution plan, showing you the actions Terraform will take to achieve the desired state described in your configuration.Good idea to use it before terraform apply.

<br/>

## terraform validate
Validates the Terraform configuration files.Checks the syntax of the configuration files and makes sure they are valid, but doesn't interact with the actual infrastructure.Good idea to use it before terraform apply.

<br/>

## terraform apply
Applies the changes required to reach the desired state of the configuration. It executes the actions planned in the terraform plan step. It will create, modify, or delete resources as needed to match the configuration.

<br/>

## terraform show
This command is used to display the current state of the Terraform-managed infrastructure. It shows the configuration that is currently stored in the state file (which is .tfstate by default), representing your infrastructure as it exists at the moment.**If you haven't used terraform apply (and have an actual tf file that does something) before using this command you will get this error.**

**The state file is empty. No resources are represented.**

<br/>

## terraform destroy
Destroys the infrastructure managed by Terraform as well as the resources (such as virtual machines, containers, etc.) that Terraform has created.

<br/>

## terraform stage
This specific command has a list of subcommands.For this exhibition let's say that we have a tf file that creates these resources.

```
docker_container.mysql
docker_container.nginx
docker_image.mysql
docker_image.nginx
docker_network.app_network
```

<br/>

#### The subcommands for the command terraform stage are
* **list**
	-  Syntax:**terraform stage list**
	-  Result:List resources in the state
	-  Output in this Example:
 ```
docker_container.mysql
docker_container.nginx
docker_image.mysql
docker_image.nginx
docker_network.app_network
 ```

<br/>

* **mv**
	-  Syntax:**terraform stage mv &lt;old address&gt; &lt;new address&gt;**
	-  Result:Changing the name or the structure of a resource


<br/>


* **pull**
	-  Syntax:**terraform stage pull**
	-  Result:The command terraform state pull is used to download the Terraform state from the remote state backend (if configured) or local state file to your machine.This command can be useful if the local state file is out of sync with the remote state, or if you want to inspect the state of your infrastructure without applying or changing anything.If you're working with a remote backend (such as AWS S3, Google Cloud Storage, etc.), the command will fetch the latest state from that backend and save it locally.If no remote backend is configured and you're using local state, it will simply display the local .tfstate file content.

<br/>

* **push**
	-  Syntax:**terraform stage push**
	-  Result:The command terraform state push is similar to terraform state pull but instead of downloading it uploads your local Terraform state file to a remote backend.**Usually it is an unnecessary command that should be avoided as terraform automatically makes the necessary changes.**

<br/>

* **replace-provider**
	-  Syntax:**terraform state replace-provider &lt;old-provider&gt; &lt;new-provider&gt;**
	-  Result:replace the provider associated with one or more resources in the Terraform state file.(you need to make sure that the new provider is defined)
  
<br/>
  
* **rm**
	-  Syntax:**terraform state rm &lt;address&gt;**
	-  Result:The terraform state rm command is used to remove resources from Terraform's state file. This command does not delete the actual resource; it simply removes it from Terraform's management, meaning Terraform will no longer track it or include it in future plans or applies.

<br/>

* **show**
	-  Syntax:**terraform state show &lt;address&gt;**
	-  Result:The terraform state show command is used to display the attributes and metadata of a specific resource in the Terraform state file.
