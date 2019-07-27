package engine;

import java.io.Serializable;
import java.util.List;

public class Profile implements Serializable {
    private String email;
    private String nome;
    private String sobrenome;
    private String residencia;
    private String formacao;
    private String habilidades;
    private List<String> experiencias;

    public Profile(String email, String nome, String sobrenome, String residencia, String formacao, String habilidades, List<String> experiencias) {
        this.email = email;
        this.nome = nome;
        this.sobrenome = sobrenome;
        this.residencia = residencia;
        this.formacao = formacao;
        this.habilidades = habilidades;
        this.experiencias = experiencias;
    }

    public String getEmail() {
        return this.nome + " " + this.sobrenome;
    }

    public String getNomeCompleto() {
        return this.nome + " " + this.sobrenome;
    }

    public String getResidencia() {
        return this.residencia;
    }

    public String getFormacao() {
        return this.formacao;
    }

    public String getHabilidades() {
        return this.habilidades;
    }

    public String getExperiencias() {

        String result = "";
        for (int i = 0; i < this.experiencias.size(); i++) {
            result += "(" + (i+1) + ") " + this.experiencias.get(i) + "\n";
        }
        return result;
    }

    public void addExperiencia(String experiencia) {
        this.experiencias.add(experiencia);
    }

    public String toString() {
        return "Email: " + this.email + "\n"
                + "Nome: " + this.nome + " Sobrenome: " + this.sobrenome + "\n"
                + "Residência: " + this.residencia + "\n"
                + "Formação Acadêmica: " + this.formacao + "\n"
                + "Habilidades: " + this.habilidades + "\n"
                + "Experiência: \n" + this.getExperiencias() + "\n";
    }
}
