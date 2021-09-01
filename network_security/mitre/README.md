# MITRE

> [Mitre Corporation - Wikipedia](https://en.wikipedia.org/wiki/Mitre_Corporation)
> [最强大的网络安全“公司”：MITRE](https://zhuanlan.zhihu.com/p/162799295)

MITRE 是一家私营、非营利性公司，主要为美国政府提供工程和技术指导, 创建于1958年，前身是麻省理工学院的林肯实验室。服务范围包含军事战争、网络太空、司法医疗等领域。运营多家政府资助的研发中心，拥有全球顶尖的技术实力，是美国政府的顶级咨询公司。参与了许多商业和最高机密项目。其中包括开发FAA空中交通管制系统和AWACS机载雷达系统等。

关于网络安全方面，大家经常能见到 CVE、OVAL、MITRE ATT&CK 等都是 MITRE 公司的产品。

### CVE

> https://cve.mitre.org/
> 
> [Common Vulnerabilities and Exposures](https://en.wikipedia.org/wiki/Common_Vulnerabilities_and_Exposures)

<mark>CVE项目的使命是识别、定义和分类登记公开披露的网络安全漏洞</mark>。该项目由由美国国土安全部 (DHS) 和网络安全和基础设施安全局 (CISA) 赞助。该项目于1999年9月正式面向公众推出。



### OVAL

> https://oval.cisecurity.org/
> 
> [Open Vulnerability and Assessment Language](https://en.wikipedia.org/wiki/Open_Vulnerability_and_Assessment_Language)
> 
> [OVAL学习笔记_bilibili的博客-CSDN博客_oval](https://blog.csdn.net/aap159951/article/details/51119093)

OVAL 旨在标准化如何评估和报告计算机系统的机器状态。 他提供了一种用来定义检查项、脆弱点等技术细节的一种描述语言。OVAL同样使用标准的XML格式组织其内容。它提供了足够的灵活性，可以用于分析Windows、Linux、Unix以及各种嵌入式操作系统的系统状态、漏洞、配置、补丁等情况，而且还能用于描述测试报告。OVAL能够清晰地对与安全相关的检查点作出描述，并且这种描述是机器可读的，能够直接应用到自动化的安全扫描中。OVAL的核心是“公开”（Open），这就意味着任何人都可以为OVAL的发展作出自己的贡献，共享知识和经验，避免重复劳动。 实际上XCCDF设计的目标是能够支持与多种基础配置检查技术交互。其中推荐的，默认的检查技术是MITRE公司的OVAL。在实际的[SCAP](https://csrc.nist.gov/projects/security-content-automation-protocol/)应用中，[XCCDF](https://csrc.nist.gov/projects/security-content-automation-protocol/specifications/xccdf)和OVAL往往是成对出现，XCCDF定义检查单，而OVAL定义每个检查项的具体实施细节。


### MITRE ATT&CK

> Adversarial Tactics Techniques and Common Knowledge framework（对抗策略技术和通用知识框架）
> 
> [MITRE ATT&CK](https://attack.mitre.org/)
> 
> https://zhuanlan.zhihu.com/p/159206890
> 
> [以ATT&CK为例构建网络安全知识图](http://blog.nsfocus.net/take-attck-as-an-example-to-build-a-network-security-knowledge-graph/)

ATT＆CK是MITRE提供的对抗策略技术和通用知识框架。 ATT&CK 知识库被用作在私营部门、政府以及网络安全产品和服务社区中开发特定威胁模型和方法的基础。

随着 ATT&CK 的创建，MITRE 正在履行其为更安全的世界解决问题的使命——通过将社区聚集在一起开发更有效的网络安全。 ATT&CK 是开放的，可供任何个人或组织免费使用。


