# Rule Finder

This File exists in case you want to check an existing rules purpose (in case you have the Rule number) but have forgotten the use of that rule.

For example if you are using Standalone Version and have declared 20 rules in the settings.json it is natural that you won't rememeber all of them by heart.

This python programme helps you by allowing you to see an existing rules purpose.

<br/>

## STEPS

1)Run the command 
```  
python <name-of-programme>.py 
```
<br/>

#### 2)These options will appear:
```
Please select a language to view its SonarLint rules:
1 - Python
2 - Java
3 - C++
4 - JavaScript
5 - TypeScript
6 - PHP
7 - Go
8 - Kotlin
9 - Ruby
10 - Swift
11 - C#
12 - C
13 - Docker
14 - Kubernetes
15 - HTML
16 - Terraform
Enter the number corresponding to the language:
```
Simply Follow the instructions

<br/>

#### 3)Finally this text will appear:
```
Enter the rule key WITHOUT LETTERS  (e.g., 107 NOT S107): 
```
Again simply Follow the instruction and once you are done the programme will redirect you to the appropriate rule in https://rules.sonarsource.com/

<br/>

Finally This is the programme(**of course you need to have python installed to run this**)

```
import webbrowser

def open_language_rules(language_number):
    # Define the mapping of numbers to language rules
    language_mapping = {
        1: "python",
        2: "java",
        3: "cpp",  # C++
        4: "javascript",
        5: "typescript",
        6: "php",
        7: "go",
        8: "kotlin",
        9: "ruby",
        10: "swift",
        11: "csharp",
        12: "c",        # C
        13: "docker",    # Docker
        14: "kubernetes", # Kubernetes
        15: "html",      # HTML
        16: "terraform"  # Terraform
    }

    # Check if the input is valid
    if language_number not in language_mapping:
        print("Invalid selection. Please choose a number between 1 and 16.")
        return None

    # Get the language name from the mapping
    language = language_mapping[language_number]
    
    return language

def open_rule_page(language, rule_key):
    # Construct the URL for the SonarSource rules page with the rule key
    url = f"https://rules.sonarsource.com/{language}/RSPEC-{rule_key}"
    
    # Open the URL in the default web browser
    webbrowser.open(url)
    print(f"Opening SonarSource rule {rule_key} for {language}...")

def main():
    # Show the available languages to the user
    print("Please select a language to view its SonarLint rules:")
    print("1 - Python")
    print("2 - Java")
    print("3 - C++")
    print("4 - JavaScript")
    print("5 - TypeScript")
    print("6 - PHP")
    print("7 - Go")
    print("8 - Kotlin")
    print("9 - Ruby")
    print("10 - Swift")
    print("11 - C#")
    print("12 - C")
    print("13 - Docker")
    print("14 - Kubernetes")
    print("15 - HTML")
    print("16 - Terraform")

    # Get the user's choice for language
    try:
        language_number = int(input("Enter the number corresponding to the language: ").strip())
        language = open_language_rules(language_number)
        
        if not language:
            return  # If invalid language number, stop execution
        
        # Ask for the rule key after selecting the language
        rule_key = input("Enter the rule key WITHOUT LETTERS  (e.g., 107 NOT S107): ").strip()

        # Open the rule page for the selected language and rule key
        open_rule_page(language, rule_key)
        
    except ValueError:
        print("Invalid input. Please enter a number.")

if __name__ == "__main__":
    main()
```
