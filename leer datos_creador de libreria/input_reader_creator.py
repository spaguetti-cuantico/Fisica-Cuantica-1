import os
import sys

def header_decorator_start():
    def wrapper(func):
        def wrapped(obj, *args, **kargs):
            obj.input_c_file.write(
'''#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef '''+ obj.name.upper() +'''_INPUT_H_INCLUDED
#define '''+ obj.name.upper() +'''_INPUT_H_INCLUDED

/**
Esta libreria ha sido creada por un sript de python creado por Unai Leria Fortea
En caso de algun problema envie un mensaje a unaileria@gmail.com
O envie su duda por el servidor de discord https://discord.gg/N48TNVF
**/

'''
            )

            func(obj, *args, **kargs)        
        return wrapped
    return wrapper

def header_decorator_finish():
    def wrapper(func):
        def wrapped(obj, *args, **kargs):
            func(obj, *args, **kargs)

            obj.input_c_file.write(
'''
    free(Fcopy);
    return 1;
}
#endif //'''+ obj.name.upper() +'''_INPUT_H_INCLUDED'''
            )
        return wrapped
    return wrapper

# TODO
def config_decorator():
    def wrapper(func):
        def wrapped(obj, *args, **kargs):
            obj.input_conf_file.write(
f'''*****************************************************************
Archivo de configuracion:
Nombre de libreria:             {obj.name + '_header.c'}
Nombre de struct:               {obj.struct_name}
Nombre de funcion de lectura:   {obj.funcion_name}

Variables:
'''
            )
            func(obj, *args, **kargs)
            obj.input_conf_file.write(
                '''*****************************************************************'''
                )
        return wrapped
    return wrapper


class imput_file():
    def __init__(self, *, header_name: str, struct_name: str):

        if not header_name:
            header_name = 'input'
        
        self.struct_name = struct_name
        self.name = header_name
        self.input_c_file = open(self.name +'_header.c','w+')
        self.input_conf_file = open(self.name +'_config.txt','w+')
        self.var_list = []
        self.funcion_name = ''


    def add_var(self, var_type, var_name, *, var_desc):

        self.var_list.append([var_type, var_name, var_desc])
        

    @header_decorator_start()
    def create_config_struct(self):

        lines = list(f"  {var[0]} {var[1]};{(' //' + var[2]) if var[2] else ''}\n" for var in self.var_list)
        lines.append(f"{'} ' + self.struct_name }; \n\n")
        self.input_c_file.write(f"typedef struct {self.struct_name.capitalize()}"+'\n{\n')
        self.input_c_file.writelines(lines)

    @header_decorator_finish()
    def create_read_funcion(self, funcion_name: str):

        self.funcion_name = funcion_name

        self.input_c_file.write(f"int {self.funcion_name}({self.struct_name} *Config)"+'\n{\n' + 
    f'''FILE *F;    
    unsigned int Fsize;
    char *Fcopy;

    F = fopen("{self.name +'_config.txt'}", "rt");
        
    fseek(F, 0, SEEK_END);
    Fsize = ftell(F);
    rewind(F);
        
    Fcopy = (char *)malloc(sizeof(char) * Fsize);
    fread(Fcopy, 1, Fsize, F);
    fclose(F);

        
    char *p;
    char s[25];
    
''')
        
        for var in self.var_list:

            frase = self.get_var_string_c_file(var)
            self.input_c_file.write(frase)


    def close(self):

        self.input_c_file.close()
        self.input_conf_file.close()
        

    @config_decorator()
    def write_conf_file(self):

        for var in self.var_list:
            line = f"{var[2] if var[2] else ''}\n"
            line += f"\t{var[0]} {var[1]}=\n"
            self.input_conf_file.write(line)
            

    def get_var_string_c_file(self, var):
        
        switch = {
            'int': f'''
    strcpy(s, "{var[1]}=");
    p = strstr(Fcopy, s);
    Config->{var[1]} = atoi(p + strlen(s));
    if (0)
    {'{'}
        printf("ERROR:valor de {var[1]} invalido \\n'");
        free(Fcopy);
        return 0;
    {'}'}\n\n''',
            'double': f'''
    strcpy(s, "{var[1]}=");
    p = strstr(Fcopy, s);
    Config->{var[1]} = atof(p + strlen(s));
    if (0)
    {'{'}
        printf("ERROR:valor de {var[1]} invalido\\n");
        free(Fcopy);
        return 0;
    {'}'}\n\n''',
            'char': f'''
    strcpy(s, "{var[1]}=");
    p = strstr(Fcopy, s);
    Config->{var[1]} = (p + strlen(s))[0];
    if (0)
    {'{'}
        printf("ERROR:valor de {var[1]} invalido\\n");
        free(Fcopy);
        return 0;
    {'}'}\n\n''',
        }
        try:
            return switch[var[0]]
        except:
            return ''


class user_interface():
    
    def __init__(self):
        self.input_name = self.ask_user('Introduce el nombre deseado de la libreria a crear:')
        if not self.input_name:
            sys.exit('Es necesario el nombre de la libreria')
    
        self.struct_name = self.ask_user(f"Introduce el nombre deseado del Struct con las variables que leera {self.input_name}:")
        if not self.struct_name:
            sys.exit('Es necesario el nombre del Struct')

        self.library = imput_file(
            header_name=self.input_name,
            struct_name=self.struct_name
            )
    
        i = 1
        print(
'''VARIABLES: 
Ahora escriba una por una las variables que quiere que lea su programa de la siguiente manera: 
Ej/:
int contador Esto es una prueba
double media Esto tambien

tipo nombre descripcion   (la descripcion no es obligatoria)''')

        while True:
            
            variable = self.ask_user(f"Introduce el tipo y el nombre de la varialbe Nº{i}:")
            if not variable:
                break

            args = variable.split(maxsplit=2)
            var_type = ''
            var_name = ''
            var_desc = ''
            try:
                var_type, var_name, var_desc = args
            except:
                try:
                    var_type, var_name = args
                except:
                    continue

            if not var_type or not var_name:
                break
            else:
                self.library.add_var(var_type, var_name, var_desc=var_desc)

                i += 1

        if i == 1:
            sys.exit('Al menos debe de haber una variable')

        self.library.create_config_struct()
        self.function_name = self.ask_user(f"Introduce el nombre deseado de la funcion que leera los datos de configuracion:")
        if not self.function_name:
            sys.exit('Es necesario el nombre de la funcion que lee la configuracion')

        self.library.create_read_funcion(self.function_name)
        self.library.write_conf_file()
        

    def close(self):

        self.library.close()
        sys.exit('Archivos creados')


    def ask_user(self, question: str) -> str:
        try:
            if question:
                print(question)
            awnser = input()
            return awnser
        except EOFError or RuntimeError:
            return None



if __name__ == "__main__":
    interface = user_interface()
    interface.close()