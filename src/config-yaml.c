/*
** Copyright (C) 2018 Quadrant Information Security <quadrantsec.com>
** Copyright (C) 2018 Champ Clark III <cclark@quadrantsec.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License Version 2 as
** published by the Free Software Foundation.  You may not use, modify or
** distribute this program under any other version of the GNU General
** Public License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"             /* From autoconf */
#endif

#ifdef HAVE_LIBYAML
#include <yaml.h>
#endif

#ifndef HAVE_LIBYAML
** You must of LIBYAML installed! **
#endif

#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include "meer.h"
#include "meer-def.h"
#include "util.h"
#include "config-yaml.h"


struct _MeerConfig *MeerConfig;
struct _MeerOutput *MeerOutput;


void Load_YAML_Config( char *yaml_file )
{

    struct stat filecheck;

    yaml_parser_t parser;
    yaml_event_t  event;

    bool done = 0;

    unsigned char type = 0;
    unsigned char sub_type = 0;
    unsigned char toggle = 0;

    char last_pass[128] = { 0 };

    MeerConfig = malloc(sizeof(_MeerConfig));

    if ( MeerConfig == NULL )
        {
            Meer_Log(M_ERROR, "[%s, line %d] Failed to allocate memory for config. Abort!", __FILE__, __LINE__);
        }

    memset(MeerConfig, 0, sizeof(_MeerConfig));

    /* Init MeerConfig values */

    MeerConfig->interface[0] = '\0';
    MeerConfig->hostname[0] = '\0';
    MeerConfig->runas[0] = '\0';
    MeerConfig->classification_file[0] = '\0';
    MeerConfig->reference_file[0] = '\0';
    MeerConfig->genmsgmap_file[0] = '\0';
    MeerConfig->waldo_file[0] = '\0';
    MeerConfig->follow_file[0] = '\0';
    MeerConfig->lock_file[0] = '\0';

    MeerOutput = malloc(sizeof(_MeerOutput));

    if ( MeerOutput == NULL )
        {
            Meer_Log(M_ERROR, "[%s, line %d] Failed to allocate memory for output. Abort!", __FILE__, __LINE__);
        }

    memset(MeerOutput, 0, sizeof(_MeerOutput));

#ifdef HAVE_LIBMYSQLCLIENT_R

    MeerOutput->mysql_enabled = false;
    MeerOutput->mysql_debug = false;
    MeerOutput->mysql_server[0] = '\0';
    MeerOutput->mysql_port = 0;
    MeerOutput->mysql_username[0] = '\0';
    MeerOutput->mysql_password[0] = '\0';
    MeerOutput->mysql_database[0] = '\0';

#endif

    if (stat(yaml_file, &filecheck) != false )
        {
            Meer_Log(M_ERROR, "[%s, line %d] Cannot open configuration file '%s'! %s", __FILE__, __LINE__, yaml_file, strerror(errno) );
        }

    FILE *fh = fopen(yaml_file, "r");

    if (!yaml_parser_initialize(&parser))
        {
            Meer_Log(M_ERROR, "[%s, line %d] Failed to initialize the libyaml parser. Abort!", __FILE__, __LINE__);
        }

    if (fh == NULL)
        {
            Meer_Log(M_ERROR, "[%s, line %d] Failed to open the configuration file '%s' Abort!", __FILE__, __LINE__, yaml_file);
        }

    /* Set input file */

    yaml_parser_set_input_file(&parser, fh);

    while(!done)
        {

            if (!yaml_parser_parse(&parser, &event))
                {

                    /* Useful YAML vars: parser.context_mark.line+1, parser.context_mark.column+1, parser.problem, parser.problem_mark.line+1,
                       parser.problem_mark.column+1 */

                    Meer_Log(M_ERROR, "[%s, line %d] libyam parse error at line %d in '%s'", __FILE__, __LINE__, parser.problem_mark.line+1, yaml_file);
                }

            if ( event.type == YAML_DOCUMENT_START_EVENT )
                {

                    yaml_version_directive_t *ver = event.data.document_start.version_directive;

                    if ( ver == NULL )
                        {
                            Meer_Log(M_ERROR, "[%s, line %d] Invalid configuration file. Configuration must start with \"%%YAML 1.1\"", __FILE__, __LINE__);
                        }

                    int major = ver->major;
                    int minor = ver->minor;

                    if (! (major == YAML_VERSION_MAJOR && minor == YAML_VERSION_MINOR) )
                        {
                            Meer_Log(M_ERROR, "[%s, line %d] Configuration has a invalid YAML version.  Must be 1.1 or above", __FILE__, __LINE__);
                        }

                }

            else if ( event.type == YAML_STREAM_END_EVENT )
                {

                    done = true;

                }

            else if ( event.type == YAML_MAPPING_START_EVENT )
                {

                    toggle = 1;

                }

            else if ( event.type == YAML_MAPPING_END_EVENT )
                {

                    toggle = 0;
                    sub_type = 0;

                }

            else if ( event.type == YAML_SCALAR_EVENT )
                {

                    char *value = (char *)event.data.scalar.value;

                    if ( !strcmp(value, "meer-core"))
                        {
                            type = YAML_TYPE_MEER;
                        }

                    else if ( !strcmp(value, "output-plugins"))
                        {
                            type = YAML_TYPE_OUTPUT;
                        }


                    if ( type == YAML_TYPE_MEER )
                        {

                            if ( !strcmp(value, "core"))
                                {
                                    sub_type = YAML_MEER_CORE_CORE;
                                }

                        }

                    else if ( type == YAML_TYPE_OUTPUT )
                        {

                            if ( !strcmp(value, "mysql"))
                                {
                                    sub_type = YAML_MEER_MYSQL;
                                }

                        }

                    if ( type == YAML_TYPE_MEER && sub_type == YAML_MEER_CORE_CORE )
                        {

                            if ( !strcmp(last_pass, "interface" ))
                                {
                                    strlcpy(MeerConfig->interface, value, sizeof(MeerConfig->interface));
                                }

                            else if ( !strcmp(last_pass, "hostname" ))
                                {
                                    strlcpy(MeerConfig->hostname, value, sizeof(MeerConfig->hostname));
                                }

                            else if ( !strcmp(last_pass, "runas" ))
                                {
                                    strlcpy(MeerConfig->runas, value, sizeof(MeerConfig->runas));
                                }

                            else if ( !strcmp(last_pass, "classification" ))
                                {
                                    strlcpy(MeerConfig->classification_file, value, sizeof(MeerConfig->classification_file));
                                }

                            else if ( !strcmp(last_pass, "reference" ))
                                {
                                    strlcpy(MeerConfig->reference_file, value, sizeof(MeerConfig->reference_file));
                                }

                            else if ( !strcmp(last_pass, "gen-msg-map" ))
                                {
                                    strlcpy(MeerConfig->genmsgmap_file, value, sizeof(MeerConfig->genmsgmap_file));
                                }

                            else if ( !strcmp(last_pass, "waldo-file" ))
                                {
                                    strlcpy(MeerConfig->waldo_file, value, sizeof(MeerConfig->waldo_file));
                                }

                            else if ( !strcmp(last_pass, "lock-file" ))
                                {
                                    strlcpy(MeerConfig->lock_file, value, sizeof(MeerConfig->lock_file));
                                }

                            else if ( !strcmp(last_pass, "follow-eve" ))
                                {
                                    strlcpy(MeerConfig->follow_file, value, sizeof(MeerConfig->follow_file));
                                }

                        }


#ifndef HAVE_LIBMYSQLCLIENT_R

                    if ( type == YAML_TYPE_OUTPUT &&  sub_type == YAML_MEER_MYSQL )
                        {

                            if ( !strcmp(last_pass, "enabled" ))
                                {

                                    if ( !strcasecmp(value, "yes") || !strcasecmp(value, "true" ))
                                        {
                                            Meer_Log(M_ERROR, "Error.  Meer wasn't compiled with MySQL support.  Abort!");
                                        }

                                }
                        }

#endif


#ifdef HAVE_LIBMYSQLCLIENT_R

                    if ( type == YAML_TYPE_OUTPUT &&  sub_type == YAML_MEER_MYSQL )
                        {

                            if ( !strcmp(last_pass, "enabled" ))
                                {

                                    if ( !strcasecmp(value, "yes") || !strcasecmp(value, "true" ))
                                        {
                                            MeerOutput->mysql_enabled = true;
                                        }

                                }

                            else if ( !strcmp(last_pass, "debug" ) && MeerOutput->mysql_enabled == true )
                                {
                                    MeerOutput->mysql_debug = true;
                                }

                            else if ( !strcmp(last_pass, "server" ) && MeerOutput->mysql_enabled == true )
                                {
                                    strlcpy(MeerOutput->mysql_server, value, sizeof(MeerOutput->mysql_server));
                                }

                            else if ( !strcmp(last_pass, "port" ) && MeerOutput->mysql_enabled == true )
                                {
                                    MeerOutput->mysql_port = atoi(value);
                                }

                            else if ( !strcmp(last_pass, "username" ) && MeerOutput->mysql_enabled == true )
                                {
                                    strlcpy(MeerOutput->mysql_username, value, sizeof(MeerOutput->mysql_username));
                                }

                            else if ( !strcmp(last_pass, "password" ) && MeerOutput->mysql_enabled == true )
                                {
                                    strlcpy(MeerOutput->mysql_password, value, sizeof(MeerOutput->mysql_password));
                                }

                            else if ( !strcmp(last_pass, "database" ) && MeerOutput->mysql_enabled == true )
                                {
                                    strlcpy(MeerOutput->mysql_database, value, sizeof(MeerOutput->mysql_database));
                                }


                        }

#endif

                    strlcpy(last_pass, value, sizeof(last_pass));
                } /* end of else */



        }

    /* Sanity check on core configurations */

    if ( MeerConfig->interface[0] == '\0' )
        {
            Meer_Log(M_ERROR, "Configuration incomplete.  No 'interface' specified!");
        }

    if ( MeerConfig->hostname[0] == '\0' )
        {
            Meer_Log(M_ERROR, "Configuration incomplete.  No 'hostname' specified!");
        }

    if ( MeerConfig->runas[0] == '\0' )
        {
            Meer_Log(M_ERROR, "Configuration incomplete.  No 'runas' specified!");
        }

    if ( MeerConfig->classification_file[0] == '\0' )
        {
            Meer_Log(M_ERROR, "Configuration incomplete.  No 'classification' file specified!");
        }

    if ( MeerConfig->reference_file[0] == '\0' )
        {
            Meer_Log(M_ERROR, "Configuration incomplete.  No 'reference' file specified!");
        }

    if ( MeerConfig->genmsgmap_file[0] == '\0' )
        {
            Meer_Log(M_ERROR, "Configuration incomplete.  No 'gen-msg-map' file specified!");
        }

    if ( MeerConfig->waldo_file[0] == '\0' )
        {
            Meer_Log(M_ERROR, "Configuration incomplete.  No 'waldo-file' specified!");
        }

    if ( MeerConfig->follow_file[0] == '\0' )
        {
            Meer_Log(M_ERROR, "Configuration incomplete.  No 'follow-exe' file specified!");
        }

    if ( MeerConfig->lock_file[0] == '\0' )
        {
            Meer_Log(M_ERROR, "Configuration incomplete.  No 'lock-file' file specified!");
        }

#ifdef HAVE_LIBMYSQLCLIENT_R

    if ( MeerOutput->mysql_enabled == true )
        {

            if ( MeerOutput->mysql_server[0] == '\0' )
                {
                    Meer_Log(M_ERROR, "MySQL output configuration incomplete.  No 'server' specified!");
                }

            if ( MeerOutput->mysql_username[0] == '\0' )
                {
                    Meer_Log(M_ERROR, "MySQL output configuration incomplete.  No 'username' specified!");
                }


            if ( MeerOutput->mysql_password[0] == '\0' )
                {
                    Meer_Log(M_ERROR, "MySQL output configuration incomplete.  No 'password' specified!");
                }

            if ( MeerOutput->mysql_database[0] == '\0' )
                {
                    Meer_Log(M_ERROR, "MySQL output configuration incomplete.  No 'database' specified!");
                }

            if ( MeerOutput->mysql_port == 0 )
                {
                    Meer_Log(M_ERROR, "MySQL output configuration incomplete.  No 'port' specified!");
                }
        }

#endif

    Meer_Log(M_NORMAL, "Configuration '%s' for host '%s' successfully loaded.", yaml_file, MeerConfig->hostname);

}
