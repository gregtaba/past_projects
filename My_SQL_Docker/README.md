# How to use docker to start ,run,backup and restore SQL databases

# Docker Compose

---

Το παρακάτω είναι το docker-compose.yml file που χρησιμοποιείς για να φτιαξεις το container mysql-server.

---

``` 
 version: '3.8'
   
 services:
    db:
     image: mysql:5.7
     container_name: mysql-server
     restart: always
     environment:
       MYSQL_ROOT_PASSWORD: yourpassword
       MYSQL_DATABASE: test_db
       MYSQL_USER: user
       MYSQL_PASSWORD: password
     ports:
       - "3306:3306"
     volumes:
       - mysql_data:/var/lib/mysql # Αποθηκεύει τα δεδομένα της βάσης στο volume
       - ./scripts:/docker-entrypoint-initdb.d # Βάζει SQL scripts για αρχικοποίηση
 
 volumes:
   mysql_data:
     driver: local 
``` 
---
---
Εντολές εκκίνησης και τερματισμού
---
> <kbd>docker compose up -d</kbd> (τρεχει το docker-compose και δημιουργει τον container)

> <kbd>docker exec -it mysql-server mysql -u root -p</kbd> (με αυτην την εντολη μπαινεις στο mySQL-Client οπου μπορεις να τρεξεις sql queries ,επίσης σου ζητειται κωδικος ,σε αυτην την περιπτωση yourpassword)

> <kbd>docker exec -it mysql-server bash</kbd> (με αυτην την εντολη μπαινεςι μεσα στο shell του container οπου τρεχεις εντολες όπως ls)

> <kbd>docker compose down</kbd> (για να τερματίσεις το container)
---
Βασικες Εντολές μεσα στο mySQL-Client
---
> <kbd>CREATE DATABASE test_db;</kbd> (Δημιουργεί ένα databse με ονομα test_db)

> <kbd>SHOW DATABASES;</kbd> σου δειχνει όλα τα databases που εχεις(καποια υπάρχουν από πριν)

> <kbd>DROP DATABASE test_db;</kbd> (διαγραφη βαση δεδομενων test_db)

> <kbd>use test_db;</kbd> (χρησιμοποιεις απο δω και περα το database test_db)

###### ολες οι εντολες όταν εισαι μεσα mySQL-Client πρεπει να τελιωνουν με ; αλλιως το mySQL-Client νομιζει ότι δεν εχεις τελιωσει την εντολη.
---
## Εκτέλεση queries μαζι με την δημιουργία του container

> <kbd>το docker-compose αρχειο όταν δημιουργήσει τον container για πρώτη φορά θα τρέξει όλα τα queries που βρίσκονται στον φάκελο scripts (πρεπει να βρισκεται στο ιδιο directory με το docker-compose).Αυτά τα queries θα είναι διαθέσιμα από δω και πέρα επίσης μεσα στον φάκελο docker-entrypoint-initdb.d μέσα στο container mysql-server)</kbd>


---
Τρέξιμο ενός SQL Script από το MySQL Shell
---
> <kbd>docker cp <path_to_programme> mysql-server:/<name_of_script></kbd> (αυτή η εντολη γινεται εκτος container και αντιγραφει το script που εχεις φτιαξει μεσα στο container σου σε αυτην την περιπτωση mysql-server)

> <kbd>SOURCE /<name_of_script>; </kbd>(αυτή η εντολη γινεται εντος container και τρεχει το script που μολις αντιγραφηκε μεσα στο Mysql-server container) 

---
Backup μιας MySQL Βάσης 
---
> (Η παρακάτω εντολή πρεπει να τρεξει μεσα στο Shell του container για να λειτουργησει)
> 
> <kbd>mysqldump -u username -p database_name > backup.sql</kbd>
> 
> Αν το όνομα της βάσης είναι test_db και ο χρήστης root:
> 
> <kbd>mysqldump -u root -p test_db > backup.sql</kbd>
> 
> <kbd>Θα σου ζητηθεί ο κωδικός χρήστη(yourpassword). </kbd>
>   
>   Μετά από αυτό, το αρχείο backup.sql θα περιέχει όλα τα δεδομένα και τη δομή της βάσης.
---
Restore μιας Βασης
---
``` 
Ένας Τρόπος για να τσεκάρεις αν εχει παρει τα δεδομενα της βασης δεδομενων (test_db) το αρχείο backup.sql ειναι να κανεις restore την βαση και να τσεκαρεις τα δεδομενα με τις παρακάτω εντολες.
 
 <kbd>CREATE DATABASE test_db_restore;</kbd> (απο mysqlclient)
 
 <kbd>mysql -u root -p test_db_restore < backup.sql</kbd> (απο shell)
 
<kbd> USE test_db_restore;</kbd> (απο mysqlclient)
 
 <kbd>SELECT * FROM users;</kbd> (απο mysqlclient)
``` 
 Αν εμφανιστούν τα δεδομένα που είχες στην αρχική βάση τότε το backup ήταν επιτυχές.
 
 Για backup όλων των βάσεων στον server χρησιμοποιεις αυτην την εντολη.
 <kbd>mysqldump -u root -p --all-databases > all_databases_backup.sql</kbd>
 
 Και για να επαναφερεις ολες τις βάσεις χρησιμοποιεις την εντολη
 <kbd>mysql -u root -p < all_databases_backup.sql</kbd>
 (θα δώσει τα ίδια ονοματα στις βασεις που θα δημιουργησει με εκεινη που ειχαν οταν εγινε το backup)
 
 Αν θέλεις να πάρεις backup μόνο τη δομή (χωρίς δεδομένα) χρησιμοποιείς την εξής εντολή:
 <kbd>mysqldump -u root -p --no-data test_db > structure_backup.sql</kbd>
---
Τεστάρισμα του volume
---
> Χάρη στο docker-compose ολα τα δεδομένα που θα δημιουργήσεις παραμένουν ακόμα και αν κλείσεις τον container σου μεσα στο volume mysql_data.Ενας απλο τρόπος για να το τσεκάρεις ειναι να κανεις τις εξης εντολές.
> 
><kbd> docker-compose up -d</kbd> (από local machine)
> 
> <kbd>docker exec -it mysql-server mysql -u root -p</kbd> (ζηταει κωδικο)
> 
> <kbd>CREATE DATABASE test_trial_db;</kbd>(από mysql_client)
> 
> <kbd>docker-compose down</kbd> (από local machine)
> 
> <kbd>docker-compose up</kbd> (από local machine)
> 
> <kbd>docker exec -it mysql-server mysql -u root -p</kbd> (ζηταει κωδικο)
> 
> <kbd>show DATABASES;</kbd> (από mysql_client)
> 
> Αν σου εμφανιστεί μεσα στα databases και test_trial_db αυτό σημαίνει ότι το volume αποθηκεύει επιτυχώς τα στοιχεία της βασης δεδομένων σου ακομα και αν κλείσεις το container που χρησιμοποιείς.
---
##   ΠΑΡΑΔΕΙΓΜΑ ΧΡΗΣΗΣ SQL
  ---
  Αρχικά θα φτιάξετε ενα φάκελο και μέσα θα τοποθετήσετε ενα κενό φάκελο που λέγεται scripts καθώς και το docker-compose αρχείο που υπάρχει πιο πάνω καθώς και αυτά τα δύο sql scripts.
  
#####   insert.sql
  
> INSERT INTO employees_info (name, surname, gender, position) VALUES
> ('John', 'Doe', 'Male', 'Software Engineer'),
> ('Jane', 'Smith', 'Female', 'HR Manager'),
> ('Mark', 'Johnson', 'Male', 'Product Manager'),
> ('Emily', 'Brown', 'Female', 'Designer'),
> ('Michael', 'Davis', 'Male', 'Sales Representative'),
> ('Sarah', 'Wilson', 'Female', 'Marketing Specialist'),
> ('David', 'Martinez', 'Male', 'Accountant'),
> ('Linda', 'Garcia', 'Female', 'Customer Support'),
> ('James', 'Taylor', 'Male', 'Developer'),
> ('Patricia', 'Anderson', 'Female', 'Operations Manager');
  
#####   alter_table.sql
  
> 
> ALTER TABLE employees_info ADD COLUMN date_started_working DATE;
> 
> UPDATE employees_info
> SET date_started_working = DATE_ADD('2010-01-01', INTERVAL (RAND() * 3650) DAY);
> 
> 
> 
  
  
  
  Στην συνέχεια μέσα στο φάκελο scripts θα βάλετε μέσα αυτό το sql script. 
  
#####   create.sql
  
> CREATE DATABASE IF NOT EXISTS company_data;
> 
> USE company_data;
> 
> CREATE TABLE IF NOT EXISTS employees_info (
>     id INT AUTO_INCREMENT PRIMARY KEY,
>     name VARCHAR(100),
>     surname VARCHAR(100),
>     gender VARCHAR(10),
>     position VARCHAR(100)
> );
  ---
  
#####   Μόλις τελιώσουν αυτά τα βήματα είμαστε έτοιμοι να τρέξουμε το πρόγραμμα.
  
>   docker compose up -d
  
  Αυτήν την στιγμή εχει δημιουργηθεί ο container μας και επίσης έχουν τρέξει ολα τα sql scripts που βρισκόντουσαν μέσα στο φάκελο scripts (άρα το create.sql).Επομένως αν πάμε να δουμε μέσα στον container τις βάσεις δεδομένων θα δούμε την βάση δεδομένων company_data.
  
>   docker exec -it mysql-server mysql -u root -p (η εντολή στο δικο μας local host machine,μας ζητάει κωδικό)
>   SHOW DATABASES; (στο mysql_client)
  
  Μεσα στις βάσεις δεδομένων θα πρέπει να εμφανιστεί και το company_data.Αν όμως ελέγξουμε τι έχει μέσα θα δουμε ότι παρότι το table employees_info εχει δημιουργηθεί δεν περιέχει τίποτα μέσα.Οπότε στην συνέχεια τρέχουμε αυτές τις εντολές στο mysql_client για να το ελεγξουμε.
  
>   use company_data;
>   Show tables;
>   select * from employees_info; (επιστρέφει Empty set άρα όντως δεν υπάρχει τιποτα μεσα στο table)
  
  Για να προσθέσουμε δεδομένα στο table της βάσης θα πρέπει να τρέξουμε το script insert.sql.Εκτελούμε τις ακόλουθες εντολές.
  
>   docker cp <path_to_programme>\insert.sql mysql-server:/insert.sql (αντιγράφει το insert.sql στο container mysql-server.
  
>   docker exec -it mysql-server bash
>   ls (για να σιγουρευτουμε οτι το insert.sql εχει αντιγραφει στο sql.)
> 	SOURCE /insert.sql;

Μετά από αυτην την εντολη το table μας πρεπει να έχει 10 καινούργια στοιχεία ,αν τρέξουμε τώρα select * from employees_info; μέσα στον mysql_client θα πρέπει να μας εμφανιστεί αυτό.
>   
>   +----+----------+----------+--------+----------------------+
> | id | name     | surname  | gender | position             |
> +----+----------+----------+--------+----------------------+
> |  1 | John     | Doe      | Male   | Software Engineer    |
> |  2 | Jane     | Smith    | Female | HR Manager           |
> |  3 | Mark     | Johnson  | Male   | Product Manager      |
> |  4 | Emily    | Brown    | Female | Designer             |
> |  5 | Michael  | Davis    | Male   | Sales Representative |
> |  6 | Sarah    | Wilson   | Female | Marketing Specialist |
> |  7 | David    | Martinez | Male   | Accountant           |
> |  8 | Linda    | Garcia   | Female | Customer Support     |
> |  9 | James    | Taylor   | Male   | Developer            |
> | 10 | Patricia | Anderson | Female | Operations Manager   |
> +----+----------+----------+--------+----------------------+
>   
>   
  




