import React, { useState, useEffect } from 'react';
import { AlertTriangle, CheckCircle, Shield, SkipForward, Wrench, ArrowLeft, Home } from 'lucide-react';
import { VulnerabilityCheck, CheckResult, CheckStatus } from '../types';
import { vulnerabilityDefinitions } from '../data/vulnerabilities';

interface SecurityCheckerProps {
  onBack: () => void;
}

export const SecurityChecker: React.FC<SecurityCheckerProps> = ({ onBack }) => {
  const [currentOS, setCurrentOS] = useState<'windows' | 'linux' | 'macos'>('windows');
  const [currentCheckIndex, setCurrentCheckIndex] = useState(0);
  const [checkResults, setCheckResults] = useState<CheckResult[]>([]);
  const [isChecking, setIsChecking] = useState(false);
  const [isCompleted, setIsCompleted] = useState(false);

  const currentChecks = vulnerabilityDefinitions[currentOS];
  const currentCheck = currentChecks[currentCheckIndex];
  const currentResult = checkResults.find(r => r.id === currentCheck?.id);

  useEffect(() => {
    // Simular detecção automática do OS
    const userAgent = navigator.userAgent.toLowerCase();
    if (userAgent.includes('windows')) setCurrentOS('windows');
    else if (userAgent.includes('mac')) setCurrentOS('macos');
    else if (userAgent.includes('linux')) setCurrentOS('linux');
  }, []);

  const simulateCheck = async (check: VulnerabilityCheck): Promise<boolean> => {
    // Simulação removida - esta é apenas uma demonstração da interface
    // A versão real (aplicação C++) faz verificações reais do sistema
    await new Promise(resolve => setTimeout(resolve, 800));
    
    // Retornar sempre vulnerável para demonstrar a interface de correção
    return true;
  };

  const handleStartCheck = async () => {
    if (currentCheckIndex >= currentChecks.length) return;

    setIsChecking(true);
    
    const isVulnerable = await simulateCheck(currentCheck);
    
    const newResult: CheckResult = {
      id: currentCheck.id,
      status: isVulnerable ? 'vulnerable' : 'safe',
      isVulnerable
    };

    setCheckResults(prev => [...prev.filter(r => r.id !== currentCheck.id), newResult]);
    setIsChecking(false);
  };

  const handleFix = async () => {
    setIsChecking(true);
    // Simular correção
    await new Promise(resolve => setTimeout(resolve, 1500));
    
    setCheckResults(prev => 
      prev.map(r => r.id === currentCheck.id ? { ...r, status: 'fixed' } : r)
    );
    setIsChecking(false);
    handleNext();
  };

  const handleSkip = () => {
    setCheckResults(prev => 
      prev.map(r => r.id === currentCheck.id ? { ...r, status: 'skipped' } : r)
    );
    handleNext();
  };

  const handleNext = () => {
    if (currentCheckIndex < currentChecks.length - 1) {
      setCurrentCheckIndex(prev => prev + 1);
    } else {
      setIsCompleted(true);
    }
  };

  const getSeverityColor = (severity: string) => {
    switch (severity) {
      case 'Alta': return 'text-red-600 bg-red-100';
      case 'Média': return 'text-orange-600 bg-orange-100';
      case 'Baixa': return 'text-yellow-600 bg-yellow-100';
      default: return 'text-gray-600 bg-gray-100';
    }
  };

  const getResultIcon = (status: CheckStatus) => {
    switch (status) {
      case 'vulnerable': return <AlertTriangle className="h-8 w-8 text-red-500" />;
      case 'safe': return <CheckCircle className="h-8 w-8 text-green-500" />;
      case 'fixed': return <CheckCircle className="h-8 w-8 text-blue-500" />;
      case 'skipped': return <SkipForward className="h-8 w-8 text-gray-500" />;
      default: return <Shield className="h-8 w-8 text-gray-400" />;
    }
  };

  if (isCompleted) {
    const totalChecks = currentChecks.length;
    const vulnerableFound = checkResults.filter(r => r.isVulnerable).length;
    const fixed = checkResults.filter(r => r.status === 'fixed').length;
    const skipped = checkResults.filter(r => r.status === 'skipped').length;

    return (
      <div className="min-h-screen bg-gray-50 py-8">
        <div className="max-w-4xl mx-auto px-4">
          <div className="bg-white rounded-xl shadow-sm p-8">
            <div className="text-center mb-8">
              <CheckCircle className="h-16 w-16 text-green-500 mx-auto mb-4" />
              <h2 className="text-3xl font-bold text-gray-900 mb-2">Verificação Concluída!</h2>
              <p className="text-gray-600">Resumo da verificação de segurança</p>
            </div>

            <div className="grid md:grid-cols-4 gap-6 mb-8">
              <div className="text-center p-4 bg-blue-50 rounded-lg">
                <div className="text-2xl font-bold text-blue-600">{totalChecks}</div>
                <div className="text-sm text-gray-600">Total de Verificações</div>
              </div>
              <div className="text-center p-4 bg-red-50 rounded-lg">
                <div className="text-2xl font-bold text-red-600">{vulnerableFound}</div>
                <div className="text-sm text-gray-600">Vulnerabilidades Encontradas</div>
              </div>
              <div className="text-center p-4 bg-green-50 rounded-lg">
                <div className="text-2xl font-bold text-green-600">{fixed}</div>
                <div className="text-sm text-gray-600">Corrigidas</div>
              </div>
              <div className="text-center p-4 bg-gray-50 rounded-lg">
                <div className="text-2xl font-bold text-gray-600">{skipped}</div>
                <div className="text-sm text-gray-600">Ignoradas</div>
              </div>
            </div>

            <div className="space-y-3 mb-8">
              {checkResults.map((result, index) => {
                const check = currentChecks.find(c => c.id === result.id);
                if (!check) return null;

                return (
                  <div key={result.id} className="flex items-center space-x-4 p-4 bg-gray-50 rounded-lg">
                    {getResultIcon(result.status)}
                    <div className="flex-1">
                      <h4 className="font-semibold">{check.name}</h4>
                      <p className="text-sm text-gray-600">{check.description}</p>
                    </div>
                    <span className={`px-3 py-1 rounded-full text-xs font-medium ${getSeverityColor(check.severity)}`}>
                      {check.severity}
                    </span>
                    <span className="text-sm font-medium capitalize">
                      {result.status === 'vulnerable' && 'Vulnerável'}
                      {result.status === 'safe' && 'Seguro'}
                      {result.status === 'fixed' && 'Corrigido'}
                      {result.status === 'skipped' && 'Ignorado'}
                    </span>
                  </div>
                );
              })}
            </div>

            <div className="text-center">
              <button
                onClick={onBack}
                className="bg-blue-600 text-white px-8 py-3 rounded-lg font-semibold hover:bg-blue-700 transition-colors flex items-center space-x-2 mx-auto"
              >
                <Home className="h-5 w-5" />
                <span>Voltar ao Início</span>
              </button>
            </div>
          </div>
        </div>
      </div>
    );
  }

  return (
    <div className="min-h-screen bg-gray-50 py-8">
      <div className="max-w-4xl mx-auto px-4">
        {/* Header */}
        <div className="flex items-center justify-between mb-8">
          <button
            onClick={onBack}
            className="flex items-center space-x-2 text-gray-600 hover:text-gray-900 transition-colors"
          >
            <ArrowLeft className="h-5 w-5" />
            <span>Voltar</span>
          </button>
          <div className="flex items-center space-x-4">
            <select
              value={currentOS}
              onChange={(e) => setCurrentOS(e.target.value as any)}
              className="border rounded-lg px-3 py-2"
            >
              <option value="windows">Windows</option>
              <option value="linux">Linux</option>
              <option value="macos">macOS</option>
            </select>
          </div>
        </div>

        {/* Progress */}
        <div className="mb-8">
          <div className="flex justify-between text-sm text-gray-600 mb-2">
            <span>Progresso</span>
            <span>{currentCheckIndex + 1} de {currentChecks.length}</span>
          </div>
          <div className="w-full bg-gray-200 rounded-full h-2">
            <div 
              className="bg-blue-600 h-2 rounded-full transition-all duration-300"
              style={{ width: `${((currentCheckIndex + 1) / currentChecks.length) * 100}%` }}
            ></div>
          </div>
        </div>

        {/* Current Check */}
        <div className="bg-white rounded-xl shadow-sm p-8">
          <div className="text-center mb-8">
            <Shield className="h-12 w-12 text-blue-600 mx-auto mb-4" />
            <h2 className="text-2xl font-bold text-gray-900 mb-2">{currentCheck.name}</h2>
            <span className={`inline-block px-3 py-1 rounded-full text-sm font-medium ${getSeverityColor(currentCheck.severity)}`}>
              Severidade: {currentCheck.severity}
            </span>
          </div>

          <div className="mb-8">
            <h3 className="font-semibold mb-2">Descrição:</h3>
            <p className="text-gray-600 mb-4">{currentCheck.description}</p>
            
            <h3 className="font-semibold mb-2">Impacto:</h3>
            <p className="text-gray-600">{currentCheck.impact}</p>
          </div>

          {/* Check Result */}
          {currentResult && (
            <div className="mb-8 p-6 border rounded-lg">
              <div className="flex items-center justify-center space-x-4 mb-4">
                {getResultIcon(currentResult.status)}
                <div className="text-center">
                  {currentResult.status === 'vulnerable' && (
                    <div>
                      <h3 className="text-xl font-bold text-red-600">Vulnerabilidade Encontrada!</h3>
                      <p className="text-gray-600">Este sistema está vulnerável a esta ameaça.</p>
                    </div>
                  )}
                  {currentResult.status === 'safe' && (
                    <div>
                      <h3 className="text-xl font-bold text-green-600">Sistema Seguro</h3>
                      <p className="text-gray-600">Esta verificação passou com sucesso.</p>
                    </div>
                  )}
                </div>
              </div>

              {currentResult.status === 'vulnerable' && (
                <div className="bg-gray-50 p-4 rounded-lg">
                  <h4 className="font-semibold mb-2">Comando de Correção:</h4>
                  <code className="block bg-gray-800 text-gray-100 p-3 rounded text-sm font-mono">
                    {currentCheck.fix}
                  </code>
                </div>
              )}
            </div>
          )}

          {/* Action Buttons */}
          <div className="flex justify-center space-x-4">
            {!currentResult && (
              <button
                onClick={handleStartCheck}
                disabled={isChecking}
                className="bg-blue-600 text-white px-8 py-3 rounded-lg font-semibold hover:bg-blue-700 disabled:opacity-50 disabled:cursor-not-allowed transition-colors flex items-center space-x-2"
              >
                <Shield className="h-5 w-5" />
                <span>{isChecking ? 'Verificando...' : 'Iniciar Verificação'}</span>
              </button>
            )}

            {currentResult?.status === 'vulnerable' && (
              <>
                <button
                  onClick={handleFix}
                  disabled={isChecking}
                  className="bg-green-600 text-white px-8 py-3 rounded-lg font-semibold hover:bg-green-700 disabled:opacity-50 disabled:cursor-not-allowed transition-colors flex items-center space-x-2"
                >
                  <Wrench className="h-5 w-5" />
                  <span>{isChecking ? 'Corrigindo...' : 'Corrigir e Continuar'}</span>
                </button>
                <button
                  onClick={handleSkip}
                  disabled={isChecking}
                  className="bg-gray-600 text-white px-8 py-3 rounded-lg font-semibold hover:bg-gray-700 disabled:opacity-50 disabled:cursor-not-allowed transition-colors flex items-center space-x-2"
                >
                  <SkipForward className="h-5 w-5" />
                  <span>Pular sem Corrigir</span>
                </button>
              </>
            )}

            {currentResult?.status === 'safe' && (
              <button
                onClick={handleNext}
                className="bg-blue-600 text-white px-8 py-3 rounded-lg font-semibold hover:bg-blue-700 transition-colors"
              >
                Continuar
              </button>
            )}
          </div>
        </div>
      </div>
    </div>
  );
};