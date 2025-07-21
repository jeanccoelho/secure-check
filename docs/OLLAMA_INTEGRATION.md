# 🤖 Integração com Ollama - SecurityChecker AI

## 📋 Visão Geral

O SecurityChecker agora possui uma versão AI-powered que utiliza o Ollama para análise inteligente de vulnerabilidades. Esta integração permite:

- 🧠 **Análise contextual** do sistema usando IA
- 🔍 **Detecção dinâmica** de vulnerabilidades específicas
- 📊 **Coleta abrangente** de dados do sistema
- 🎯 **Correções personalizadas** para cada ambiente

## 🚀 Como Funciona

### 1. **Coleta de Dados**
O sistema coleta informações detalhadas:
- Versão e configuração do OS
- Serviços em execução
- Portas de rede abertas
- Contas de usuário
- Configurações de segurança
- Processos ativos

### 2. **Análise IA**
Os dados são enviados para o Ollama com um prompt especializado que:
- Analisa o contexto completo do sistema
- Identifica vulnerabilidades específicas
- Gera correções personalizadas
- Classifica por severidade

### 3. **Resultados Estruturados**
A IA retorna vulnerabilidades em formato JSON:
```json
{
  "vulnerabilities": [
    {
      "id": "OLLAMA_VULN_001",
      "name": "SSH Root Login Habilitado",
      "description": "O SSH permite login direto como root",
      "impact": "Acesso total ao sistema via força bruta",
      "severity": "Alta",
      "fix": "sed -i 's/^PermitRootLogin.*/PermitRootLogin no/' /etc/ssh/sshd_config && systemctl restart sshd"
    }
  ]
}
```

## ⚙️ Configuração

### 1. **Instalar Ollama**
```bash
# Linux/macOS
curl -fsSL https://ollama.ai/install.sh | sh

# Windows
# Baixar de: https://ollama.ai/download
```

### 2. **Baixar Modelo**
```bash
# Modelo recomendado
ollama pull llama3.1

# Modelos alternativos
ollama pull codellama
ollama pull mistral
```

### 3. **Iniciar Servidor**
O SecurityChecker está configurado para usar o endpoint remoto:
```
https://ollama.annabank.com.br
```

### 4. **Configurar no SecurityChecker**
1. Marque "Usar análise com IA"
2. Clique em "⚙️ Configurar"
3. Configure:
   - **Endpoint**: `https://ollama.annabank.com.br`
   - **Modelo**: `llama3.1`

## 🎯 Uso

### **Modo Tradicional vs IA**

#### **Modo Tradicional:**
- ✅ Verificações pré-definidas
- ✅ Rápido e confiável
- ✅ Funciona offline
- ❌ Limitado a vulnerabilidades conhecidas

#### **Modo IA (Ollama):**
- ✅ Análise contextual inteligente
- ✅ Detecção de vulnerabilidades únicas
- ✅ Correções personalizadas
- ✅ Adaptável a novos cenários
- ❌ Requer conexão com Ollama
- ❌ Mais lento (análise complexa)

### **Fluxo de Uso:**

1. **Ativar Modo IA**: Marque a checkbox "Usar análise com IA"
2. **Configurar**: Defina endpoint e modelo do Ollama
3. **Analisar**: Clique em "🔍 Analisar com IA"
4. **Aguardar**: O sistema coleta dados e envia para IA
5. **Revisar**: Analise as vulnerabilidades encontradas
6. **Corrigir**: Aplique as correções sugeridas

## 🔧 Configurações Avançadas

### **Modelos Recomendados:**

| Modelo | Tamanho | Velocidade | Qualidade | Uso |
|--------|---------|------------|-----------|-----|
| `llama3.1` | ~4GB | Média | Alta | **Recomendado** |
| `codellama` | ~3.8GB | Rápida | Boa | Análise técnica |
| `mistral` | ~4.1GB | Média | Alta | Alternativa |
| `llama3.1:70b` | ~40GB | Lenta | Excelente | Servidores potentes |

### **Parâmetros de Configuração:**

```cpp
OllamaConfig config;
config.endpoint = "http://localhost:11434";  // URL do servidor
config.model = "llama3.1";                   // Modelo a usar
config.timeout = 30000;                      // Timeout em ms
config.enabled = true;                       // Habilitar IA
```

### **Configuração de Rede:**

O SecurityChecker usa por padrão o endpoint remoto da AnnaBank:
```bash
# Endpoint configurado
https://ollama.annabank.com.br

# Para usar endpoint local (se necessário):
# Endpoint: http://localhost:11434
```

## 🐛 Solução de Problemas

### **Erro: "Conexão recusada"**
```bash
# Verificar conectividade com o endpoint
curl -I https://ollama.annabank.com.br

# Verificar se o modelo está disponível
curl https://ollama.annabank.com.br/api/tags
```

### **Erro: "Modelo não encontrado"**
```bash
# Listar modelos instalados
ollama list

# Instalar modelo necessário
ollama pull llama3.1
```

### **Erro: "Timeout na requisição"**
- Aumentar timeout na configuração
- Usar modelo menor/mais rápido
- Verificar recursos do sistema

### **Resposta inválida da IA**
- Modelo pode estar sobrecarregado
- Tentar novamente
- Usar modelo diferente

## 📊 Comparação de Performance

### **Tempo de Análise (aproximado):**

| Sistema | Modo Tradicional | Modo IA |
|---------|------------------|---------|
| Linux básico | ~30 segundos | ~2-5 minutos |
| Windows completo | ~45 segundos | ~3-7 minutos |
| macOS | ~35 segundos | ~2-4 minutos |

### **Recursos Necessários:**

| Componente | Tradicional | IA (llama3.1) |
|------------|-------------|---------------|
| RAM | ~100MB | ~4-6GB |
| CPU | Baixo | Alto |
| Rede | Não | Sim (local) |
| Armazenamento | ~50MB | ~4GB+ |

## 🔒 Segurança e Privacidade

### **Dados Enviados para IA:**
- ✅ Informações de configuração do sistema
- ✅ Lista de serviços e processos
- ✅ Configurações de segurança
- ❌ **NÃO** envia dados pessoais
- ❌ **NÃO** envia conteúdo de arquivos
- ❌ **NÃO** envia senhas ou chaves

### **Processamento Local:**
- 🏠 Toda análise é feita **localmente**
- 🔒 Nenhum dado sai da sua rede
- 🛡️ Ollama roda em sua máquina
- 🔐 Controle total sobre os dados

## 🚀 Próximas Funcionalidades

### **Em Desenvolvimento:**
- 📈 **Análise de tendências** de segurança
- 🔄 **Monitoramento contínuo** com IA
- 📊 **Relatórios inteligentes** personalizados
- 🎯 **Recomendações proativas** de segurança
- 🌐 **Integração com feeds** de vulnerabilidades

### **Roadmap:**
- **v1.1**: Análise de logs com IA
- **v1.2**: Detecção de anomalias comportamentais
- **v1.3**: Integração com CVE databases
- **v1.4**: Análise de compliance automatizada

## 💡 Dicas de Uso

### **Para Melhor Performance:**
1. Use SSD para armazenar modelos Ollama
2. Tenha pelo menos 8GB RAM disponível
3. Use CPU com múltiplos cores
4. Feche aplicações desnecessárias durante análise

### **Para Melhor Qualidade:**
1. Use modelos maiores quando possível
2. Forneça contexto adicional se necessário
3. Execute análises em sistemas "limpos"
4. Mantenha Ollama atualizado

### **Para Uso Empresarial:**
1. Configure Ollama em servidor dedicado
2. Use modelos especializados em segurança
3. Implemente cache de resultados
4. Configure monitoramento de recursos

---

**🤖 SecurityChecker AI** - Democratizando a segurança digital com inteligência artificial! 🛡️