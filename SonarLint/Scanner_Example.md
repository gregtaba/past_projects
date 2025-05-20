# Scanner Example

<br/>


This page will provide you with a simple example of how to run a sonar scan for the first time so you can bind a local project to a sonarqube server.

<br/>

## 1)Download the latest SonarQube server and SonarScanner Docker images




<br/>

**Pull both images using Docker:**

```
docker pull sonarqube:latest
docker pull sonarsource/sonar-scanner-cli:latest
```
<br/>

## 2)Create Docker Compose file for the server container

In the docker compose file we will also use a docker network so the two containers can communicate with each other.

**This docker compose file has to be in a different project than the scanner docker compose file we will create later.**

```

version: '3'

services:
  sonarqube:
    image: sonarqube:latest
    container_name: sonarqube
    ports:
      - "9000:9000"  # Expose SonarQube on port 9000
    environment:
      - SONARQUBE_JDBC_URL=jdbc:postgresql://db:5432/sonar
      - SONARQUBE_JDBC_USERNAME=sonar
      - SONARQUBE_JDBC_PASSWORD=sonar
    depends_on:
      - db
    networks:
      - sonarnet
    volumes:
      - sonarqube_data:/opt/sonarqube/data
      - sonarqube_extensions:/opt/sonarqube/extensions
      - sonarqube_logs:/opt/sonarqube/logs

  db:
    image: postgres:alpine
    container_name: sonarqube_db
    environment:
      - POSTGRES_USER=sonar
      - POSTGRES_PASSWORD=sonar
      - POSTGRES_DB=sonar
    networks:
      - sonarnet
    volumes:
      - postgres_data:/var/lib/postgresql/data

networks:
  sonarnet:
    driver: bridge

volumes:
  sonarqube_data:
  sonarqube_extensions:
  sonarqube_logs:
  postgres_data:
  
```

<br/>

## 3) Run the command ` docker network ls `

Once you do locate the name of the network that the server docker compose file uses.The reason we do that is that sometimes docker can add the name of the project or directory the docker compose file is in, in front of the network's name.For example if the docker compose file is in the directory example then the network name might be sonarnet or example_sonarnet.

Once you locate the correct name we go to the next step

<br/>

## 4)Start the server container

Use the command ` docker compose up -d ` to start the container.(you have to be in the directory where this docker compose file is located when running the command)

Enter the server and **create an authentication token and SAVE IT**.(Instructions for that are in the intro sonarlint file in this wiki page https://wiki.prosvasis.itml.space/en/Development/DevOps/SonarQube/SonarLint)

<br/>

## 5)create a simple sonar-project.properties file

You will create the sonar-project.properties file inside the project you want to run the scan on.

This is the way the file is constructed

```
sonar.projectKey=<my_project_key>
sonar.sources=.
sonar.host.url=<Server URL>
sonar.login=<TOKEN>
```

* **&lt;my_project_key&gt; = This will be the name of the project in sonarqube server once the scan is complete.**
* **&lt;Server URL&gt; = The URL where the server container runs**
* **&lt;TOKEN&gt; = Value of the authentication token you created in step 4**

<br/>

## 6)Create Docker Compose file for the scanner container

<br/>

Now we are ready to create the docker compose file for the scanner.

```
version: '3.8'

services:
  sonar-scanner:
    image: sonarsource/sonar-scanner-cli:latest
    container_name: sonar_scanner
    environment:
      - SONAR_HOST_URL=<SERVER URL>  # Correct SonarQube server URL, no trailing slash
      - SONAR_LOGIN=<TOKEN>  # Replace with your SonarQube authentication token
    volumes:
      - .:/usr/src  # Mount your project directory into the container (adjust path accordingly)
    networks:
      - <NAME_OF_NETWORK>  # Ensure it's on the same network as SonarQube
    entrypoint: ["sonar-scanner", "-Dsonar.projectKey=example", "-Dsonar.sources=."]  # Replace with your project key and path to sources

networks:
  <NAME_OF_NETWORK>:
    external: true  # Use the same network defined in the SonarQube compose file
```

Replace these variables with the correct answers:

* **&lt;Server URL&gt; = The URL where the server container runs**
* **&lt;TOKEN&gt; = Value of the authentication token you created in step 4**

<br/>

## 7)Start the scanner container

Use the command ` docker compose up -d ` to start the container.(you have to be in the directory where this docker compose file is located when running the command)

<br/>

## 8)Read the scanner logs

Immidiately after starting the scanner container use this command

``` 
docker logs -f sonar_scanner 
```

The logs of the scanner should appear.

Once they are done you should see a message like this in the end.

``` 
SonarScanner Engine completed successfully
07:44:05.814 INFO  EXECUTION SUCCESS
07:44:05.816 INFO  Total time: 18.314s
``` 

<br/>

## 9)Reenter the server container

Once the scanning process is over reenter the server and go to **Projects**.There you will see a new project named after the **<my_project_key>** variable from step 5.

<br/>
<br/>

## 10)Bind the local project to the one in Sonarqube

Open VS CODE and go to SonarQube > Connected Mode and **hit the + symbol** next to the sonarqube server you are connected to.

Then in the command palette the option will appear **to select the remote project with which you want to bind your local folder**.There should only be one choice.The project named after the **<my_project_key>** variable.

Once you select it you have succesfully binded your project to the Sonarqube Server.

