#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <gio/gio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_ERROR_MSG 0x1000

static void trigger(GSimpleAction *action,
                    GVariant *parameter,
                    gpointer window)
{
    const gchar *value = g_variant_get_string(parameter, NULL);
    g_print("triggered: %s\n", value);
}

static GActionEntry app_entries[] = {{"trigger", trigger, "s", NULL, NULL}};

static void activate(GApplication *application)
{
    g_application_hold(application);

    g_action_map_add_action_entries(G_ACTION_MAP(application),
                                    app_entries, G_N_ELEMENTS(app_entries),
                                    application);

    // notification customizing
    GNotification *notification = g_notification_new("Lunch is ready");
    g_notification_set_body(notification, "Today we have pancakes and salad, and fruit and cake for dessert");

    // notification actions
    g_notification_set_default_action(notification, "app.go-to-lunch");
    g_notification_add_button_with_target(notification, "Trigger", "app.trigger", "s", "data");

    // notification run
    g_application_send_notification(application, "lunch-is-ready", notification);

    g_application_release(application);
}

static char *first_match(regex_t *r, const char *to_match)
{
    /* "N_matches" is the maximum number of matches allowed. */
    const int n_matches = 10;
    /* "M" contains the matches found. */
    regmatch_t m[n_matches];
    int i = 0;
    int nomatch = regexec(r, to_match, n_matches, m, 0);
    if (nomatch)
    {
        printf("No more matches.\n");
        return nomatch;
    }
    for (i = 1; i < n_matches; i++)
    {
        int start;
        int finish;
        if (m[i].rm_so == -1)
        {
            break;
        }
        start = m[i].rm_so + (to_match - to_match);
        finish = m[i].rm_eo + (to_match - to_match);
        int _strlen = (finish - start);
        char *_str = to_match + start;
        _str[_strlen] = '\0';
        return _str;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int opt = 0;

    char *user_address;
    char *user_password;
    char *mail_server;

    while ((opt = getopt(argc, argv, "u:p:h:")) != -1)
    {
        switch (opt)
        {
        case 'u':
            user_address = optarg;
            break;
        case 'p':
            user_password = optarg;
            break;
        case 'h':
            mail_server = optarg;
            break;
        }
    }

    if (user_address == NULL || user_password == NULL || mail_server == NULL)
    {
        printf("Unknown credentials!\n");
        return 1;
    }

    printf("user %s password %s server %s\n", user_address, user_password, mail_server);

    struct sockaddr_in server;
    struct hostent *host;
    int sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("Unable to create socket: %d.", errno);
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(143);
    host = gethostbyname(mail_server);
    if (host == NULL)
    {
        printf("Failed on gethostbyname: %s.", hstrerror(h_errno));
        exit(EXIT_FAILURE);
    }
    memcpy(&server.sin_addr, host->h_addr, host->h_length);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("Failed on connecting to server socket.");
        return -1;
    }

    char *message[512];
    char *buffer[512];
    for (;;)
    {
        int packet_length = recv(sock, buffer, 512, 0);
        buffer[packet_length] = '\0';
        printf("%s", buffer);
        if (packet_length != 512)
        {
            break;
        }
    }
    printf("\n");

    sprintf(message, "1 LOGIN %s %s\r\n", user_address, user_password);
    if (write(sock, message, strlen(message)) < 0)
    {
        printf("Unable to send message.");
        return -1;
    }

    for (;;)
    {
        int packet_length = recv(sock, buffer, 512, 0);
        buffer[packet_length] = '\0';
        printf("%s", buffer);
        if (packet_length != 512)
        {
            break;
        }
    }
    printf("\n");

    sprintf(message, "1 STATUS INBOX (UNSEEN)\r\n");
    if (write(sock, message, strlen(message)) < 0)
    {
        printf("Unable to send message.");
        return -1;
    }

    int packet_length = recv(sock, buffer, 512, 0);
    buffer[packet_length] = '\0';

    printf("%s\n", buffer);

    regex_t r;
    const char *regex_text = ".*\\(UNSEEN ([[:digit:]]+)\\).*";
    int status = regcomp(&r, regex_text, REG_EXTENDED | REG_NEWLINE);
    if (status != 0)
    {
        char error_message[MAX_ERROR_MSG];
        regerror(status, &r, error_message, MAX_ERROR_MSG);
        printf("Regex error compiling '%s': %s\n",
               regex_text, error_message);
        return 1;
    }

    const int unseen = atoi(first_match(&r, buffer));

    printf("match: %d\n", unseen);

    regfree(&r);
    close(sock);

    GApplication *app;
    int g_status;

    app = g_application_new("it.davidepucci.MailMan",
                            G_APPLICATION_HANDLES_OPEN);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    // g_application_set_inactivity_timeout(app, 10000);

    g_status = g_application_run(app, NULL, NULL);

    g_object_unref(app);

    return g_status;
}
