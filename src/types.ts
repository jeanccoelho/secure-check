export interface VulnerabilityCheck {
  id: string;
  name: string;
  description: string;
  impact: string;
  severity: 'Alta' | 'Média' | 'Baixa';
  fix: string;
}

export interface VulnerabilityDefinitions {
  windows: VulnerabilityCheck[];
  linux: VulnerabilityCheck[];
  macos: VulnerabilityCheck[];
}

export type CheckStatus = 'pending' | 'checking' | 'vulnerable' | 'safe' | 'skipped' | 'fixed';

export interface CheckResult {
  id: string;
  status: CheckStatus;
  isVulnerable?: boolean;
}