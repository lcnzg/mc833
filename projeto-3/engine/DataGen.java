package engine;

import java.util.HashMap;
import java.util.List;
import java.util.LinkedList;
import java.io.*;
import engine.Profile;

public class DataGen {

    public static void main(String[] args) {

        HashMap<String,Profile> map = new HashMap<String,Profile>();

        // Instanciate the profiles
        // Profile 1
        List<String> profile_exp = new LinkedList<String>();
        profile_exp.add("Servidores Linux");
        profile_exp.add("Segurança em redes");
        profile_exp.add("Criptografia PGP");
        Profile profile = new Profile("carlos@unicamp.br", "Carlos", "Merenda", "Campinas", "Engenharia de Computação", "Criptografia, Servidores", profile_exp);
        map.put("carlos@unicamp.br", profile);

        // Profile 2
        profile_exp = new LinkedList<String>();
        profile_exp.add("Redesign de interfaces");
        profile_exp.add("Acessibilidade em sites");
        profile = new Profile("joao@unicamp.br", "João", "Santos", "São Paulo", "Ciência da Computação", "UI, Acessibilidade", profile_exp);
        map.put("joao@unicamp.br", profile);

        // Profile 3
        profile_exp = new LinkedList<String>();
        profile_exp.add("Machine learning");
        profile_exp.add("Acessibilidade em sites");
        profile_exp.add("Sistemas distribuídos");
        profile_exp.add("Deep learning");
        profile = new Profile("maria@unicamp.br", "Maria", "Albergue", "Campinas", "Engenharia da Computação", "Boas soft skills, poliglota", profile_exp);
        map.put("maria@unicamp.br", profile);

        // Profile 4
        profile_exp = new LinkedList<String>();
        profile = new Profile("sitio@unicamp.br", "Sitio", "Coito", "Jundiaí", "Engenharia da Elétrica", "Nenhuma", profile_exp);
        map.put("sitio@unicamp.br", profile);

        // Profile 5
        profile_exp = new LinkedList<String>();
        profile_exp.add("Web development");
        profile_exp.add("Analista de Segurança");
        profile = new Profile("carlos@unicamp.br", "Carlos", "Martini", "Vinhedo", "Ciência da Computação", "Especialista em otimizações de serviços web", profile_exp);
        map.put("carlos@unicamp.br", profile);

        // Profile 6
        profile_exp = new LinkedList<String>();
        profile_exp.add("Cloud computing");
        profile_exp.add("Analista de sistemas");
        profile_exp.add("Desing de interfaces");
        profile_exp.add("Engenharia de algorítimos");
        profile_exp.add("Computação gráfica");
        profile_exp.add("Engenharia de redes");
        profile_exp.add("Otimização de banco de dados");
        profile_exp.add("Assessor fincanceiro");
        profile = new Profile("tiago@unicamp.br", "Tiago", "Topper", "Londres", "Letras", "Escritor, compositor e bilingue", profile_exp);
        map.put("tiago@unicamp.br", profile);

        // Profile 7
        profile_exp = new LinkedList<String>();
        profile_exp.add("Análise de genomas");
        profile_exp.add("Inteligência artificial");
        profile = new Profile("erick@unicamp.br", "Erick", "Senpai", "Japão", "Ciência da Computação", "Durmir por 37 horas seguidas", profile_exp);
        map.put("erick@unicamp.br", profile);

        // Profile 8
        profile_exp = new LinkedList<String>();
        profile_exp.add("Professora no rio branco");
        profile_exp.add("Aulas infantis");
        profile = new Profile("marina@unicamp.br", "Marina", "Gauche", "Casa branca", "Faculdade de educação física", "Especializada em lecionar crianças", profile_exp);
        map.put("marina@unicamp.br", profile);

        // Profile 9
        profile_exp = new LinkedList<String>();
        profile_exp.add("Estágio SIDI");
        profile_exp.add("Analista de malwares");
        profile = new Profile("luca@unicamp.br", "Luca", "Luca", "Campinas", "Engenharia da Computação", "Segurança de software e hardware, poliglota", profile_exp);
        map.put("luca@unicamp.br", profile);

        // Profile 10
        profile_exp = new LinkedList<String>();
        profile_exp.add("Integrante do Gamux");
        profile_exp.add("Aficcionado por hackatons");
        profile = new Profile("fabio@unicamp.br", "Fabio", "Santos", "Campinas", "Engenharia da Computação", "Desenvolvimento de jogos", profile_exp);
        map.put("fabio@unicamp.br", profile);

        // Serialization
        try {
            FileOutputStream fos =
            new FileOutputStream("data.ser");
            ObjectOutputStream oos = new ObjectOutputStream(fos);
            oos.writeObject(map);
            oos.close();
            fos.close();
        } catch(IOException ioe) {
            ioe.printStackTrace();
            return;
        }

        return;
    }
}
