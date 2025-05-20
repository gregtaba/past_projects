# SonarLint (SonarQube for IDE)

<br/>

## Intro

**SonarLint is a powerful, open-source IDE extension that helps developers detect and fix quality issues in their code as they write it**. By integrating directly with popular development environments like IntelliJ IDEA, Visual Studio, Eclipse, and VS Code, SonarLint provides real-time feedback on potential bugs, vulnerabilities, and code smells.

<br/>

**IMPORTANT:At this time SonarLint is unable to access the SonarQube Server in Teleport so you will have to use another.**

<br/>
<br/>

## Set Up (VS code)

* Open Visual Studio.

* Go to Extensions > Manage Extensions.

* Search for SonarQube for IDE in the Online tab.

* Click Download and restart Visual Studio to complete the installation.

<br/>
<br/>

## Advantages of Connecting to a SonarQube Server

Although SonarLint has a standalone mode it is much preferred to connect to a SonarQube Server as that way you are granted several advantages.

<br/>

#### Shared Quality Profiles & Rules

* **Without SonarQube Server**: SonarLint enforces only default rules locally.
* **With SonarQube Server**: Teams can define custom quality profiles and rules in SonarQube, ensuring all developers follow the same coding standards.

<br/>

#### Detect More Issues with Centralized Analysis
* **Without SonarQube:** SonarLint only performs basic local static analysis.

* **With SonarQube:** SonarQube performs deep project-wide analysis, catching more complex issues like duplicated code, security vulnerabilities, and maintainability concerns that might not be visible in an IDE.

<br/>

#### Enforce Quality Gates & Prevent Bad Code in CI/CD

* **Without SonarQube**: Developers get feedback only while coding, but no enforcement in builds.

* **With SonarQube**: Teams can set up Quality Gates, which enforce standards in pull requests, CI/CD pipelines, and production deployments—blocking bad code before it merges.

<br/>
<br/>

## How to Connect to a SonarQube Server

In order to connect to a SonarQube Server you will first need to create a token by following these steps.

* Make sure you have a SonarQube Server running , if you don't , use the latest docker image of SonarQube to create a local server.

* Enter your SonarQube Server 
* Click on your profile picture > my account > Security 
* Select a name for your token along with User Token as Type and an expiration date for your token
* Generate your token and **save your token key as this will be your only chance and you will need it later on**.

<br/>

After you create your token you are ready to establish a connection with a SonarQube Server.Fllow these steps

* Open VS CODE
* Locate and press the **SonarQube** button to your right in the Taskbar. This is what it looks like

<br/>

<img src="/sonarlint.png" alt="SonarLint" width="100">

* Under **Connected Mode** Press the button **add Sonarqube Server Connection**
* Under server URL put the url where your server is running.
* Under User Token put the token key that you saved previously when you created the token.
* Under Connection Name pick a name for your connection
* Finally press the Save Connection button.

If right next to the Save Connection button appears the message Success then you have createed a successful Connection to a SonarQube Server.

## How to use and Turn off/On SonarLint Rules

To use SonarLint Rules you have to:

* Locate and press the **SonarQube** button to your right in the Taskbar. This is what it looks like
* Press the Rules Button.
* Pick the Programming Language you want to check the rules in.Now you should be able to view all the available rules for this language.
* Every Rule has an on/off next to them as well as a **✔** or a **✖**
* If you want to deactivate them or activate them you just press the **✔** or a **✖** Icon.

<br/>

**Important:Changes to your rules are restricted to your personal development environment .To share a rule set with your team, please use Connected Mode .**



