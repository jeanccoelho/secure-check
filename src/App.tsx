import React, { useState } from 'react';
import { LandingPage } from './components/LandingPage';
import { SecurityChecker } from './components/SecurityChecker';

type AppView = 'landing' | 'checker';

function App() {
  const [currentView, setCurrentView] = useState<AppView>('landing');

  const handleStartScan = () => {
    setCurrentView('checker');
  };

  const handleBack = () => {
    setCurrentView('landing');
  };

  return (
    <div className="min-h-screen">
      {currentView === 'landing' && (
        <LandingPage onStartScan={handleStartScan} />
      )}
      {currentView === 'checker' && (
        <SecurityChecker onBack={handleBack} />
      )}
    </div>
  );
}

export default App;