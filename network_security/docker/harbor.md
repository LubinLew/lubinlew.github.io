# Harbor

Harbor 是企业级的 Registry Server 开源项目，包括了权限管理 (RBAC)、AD/LDAP、审计、管理界面、自我注册、HA 等企业必需的功能。

Harbor 支持继承大量的第三方工具, 详见 [Harbor 兼容列表]。(https://goharbor.io/docs/2.5.0/install-config/harbor-compatibility-list/)

其中 扫描器 支持

| Name                                                                         | Providers      | As Default                     |
| ---------------------------------------------------------------------------- | -------------- | ------------------------------ |
| [Clair](https://github.com/goharbor/harbor-scanner-clair)                    | CentOS         | N (removed as default in v2.2) |
| [Anchore](https://github.com/anchore/harbor-scanner-adapter)                 | Anchore        | N                              |
| [Trivy](https://github.com/aquasecurity/harbor-scanner-trivy)                | Aqua           | <mark>Y</mark>                 |
| [CSP](https://github.com/aquasecurity/harbor-scanner-aqua)                   | Aqua           | N                              |
| [DoSec](https://github.com/dosec-cn/harbor-scanner/blob/master/README_en.md) | DoSec          | N                              |
| [Sysdig Secure](https://github.com/sysdiglabs/harbor-scanner-sysdig-secure)  | Sysdig         | N                              |
| [TensorSecurity](https://github.com/tensorsecurity/harbor-scanner)           | TensorSecurity | N                              |
| [ArksecScanner](https://github.com/arksec-cn)                                | Arksec         | N                              |

2.2 之前的版本使用 Clair 作为默认扫描器, 2.2 之后使用 Trivy 作为默认扫描器。