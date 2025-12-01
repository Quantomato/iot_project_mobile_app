class Credentials(){
    private:
        string api_key = "AIzaSyBoLisW-U1NbF-yYrPxEBkI5Z0XJL2r7Vs";
        string project_id = "bollardbros-1afdb";
        string email = "juansebastianlomanto@gmail.com";
        string password = "BBA#9820821@UF";


    public:
        Credentials cred(){
            //lmao
        }

        string getEmail(){
            return email;
        }

        string getPassword(){
            return password;
        }

        string getProjectId(){
            return project_id;
        }

        string getAPIKey(){
            return api_key;
        }

}