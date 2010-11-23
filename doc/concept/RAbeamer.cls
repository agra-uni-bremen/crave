  %%  %
  %%  % $Id: RAbeamer.cls 82 2009-06-14 15:24:07Z herbert $
  %%  %
  %%  %%
  %%  %% Praesentationen mit LaTeX
  %%  %%
  %%  %% Copyright (C) 2009 Herbert Voss <hvoss@tug.org>
  %%  %% modified      2010 Finn Haedicke <finn@informatik.uni-bremen.de>
  %%  %%
  %%  %% It may be distributed and/or modified under the conditions
  %%  %% of the LaTeX Project Public License, either version 1.3
  %%  %% of this license or (at your option) any later version.
  %%  %%
  %%  %% See http://www.latex-project.org/lppl.txt for details.
  %%  %%
  %%  %
 \ProvidesClass{RAbeamer}[2010/09/07 v0.02 Vorlage Arbeitsgruppe Rechnerarchitektur Uni-Bremen]
  %%  %
\PassOptionsToPackage{ngerman,english}{babel}
\PassOptionsToPackage{table}{xcolor}
\PassOptionsToPackage{bookmarks}{hyperref}
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{beamer}}
\ProcessOptions\relax
\LoadClass[covered]{beamer}
\mode<presentation>{\beamer@suppressreplacementstrue}

\RequirePackage[T1]{fontenc}
\RequirePackage{babel}
\RequirePackage{graphicx}
\RequirePackage{tabularx}
\RequirePackage[scaled]{helvet}
%
\definecolor{RAblue}{RGB}{5,93,159}
\definecolor{RAgreen}{RGB}{153,204,0}
\definecolor{RAred}{RGB}{204,0,0}
\definecolor{RAorange}{RGB}{255,153,51}
\definecolor{RAgray}{RGB}{224,224,224}
%
  %%  \setbeamersize{text margin left=15pt,text margin right=15pt}
  %%  %
  %%  \setbeamercolor*{frametitle}{fg=black,bg=RAgray}
  %%  \setbeamercolor*{normal text}{fg=black,bg=white}
  %%  \setbeamercolor*{alerted text}{fg=RAred,bg=white}
  \setbeamercolor*{footline}{fg=white,bg=RAblue}
  \setbeamercolor*{section in toc}{parent=structure}
  %\setbeamercolor*{structure}{fg=RAblue}
  %%  \setbeamercolor*{palette primary}{use=structure,fg=structure.fg,bg=}
  %%  \setbeamercolor*{palette secondary}{fg=RAblue,bg=}
  %%  \setbeamercolor*{separation line}{fg=RAblue}
  %%  \setbeamercolor*{example text}{fg=RAgreen,bg=}
  %%  \setbeamercolor*{block title}{use=structure,fg=structure.fg,bg=RAblue!20!bg}
  %%  \setbeamercolor*{block title alerted}{use=alerted text,fg=alerted text.fg,bg=alerted text.fg!20!bg}
  %%  \setbeamercolor*{block title example}{use=example text,fg=example text.fg,bg=example text.fg!20!bg}
  %%  \setbeamercolor*{block body}{parent=normal text,use=block title,bg=block title.bg!50!bg}
  %%  \setbeamercolor*{block body alerted}{parent=normal text,use=block title alerted,bg=block title alerted.bg!50!bg}
  %%  \setbeamercolor*{block body example}{parent=normal text,use=block title example,bg=block title example.bg!50!bg}
  %%  
  %%  \setbeamerfont{tiny structure}{series=\mdseries,size=\tiny}
  %%  \setbeamerfont{title}{size=\Large,series=\bfseries}
  %%  \setbeamerfont*{author}{size=\normalsize,series=\bfseries}
  %%  \setbeamerfont{frametitle}{size=\large,series=\bfseries}
  %%  \setbeamerfont{framesubtitle}{size=\footnotesize}
  %%  \setbeamerfont*{footline}{series=\mdseries,size=\Tiny}
  %%  \setbeamerfont*{date}{size=\footnotesize}
  %%  
  %%  \setbeamertemplate{itemize item}[default]
  %%  \setbeamertemplate{sections/subsections in toc}[ball]
  %%  \setbeamertemplate{headline}{}
  %%  \setbeamertemplate{navigation symbols}{}
  %%  %\setbeamertemplate{alerted text}[default]
  %%  \beamerdefaultoverlayspecification{<+-| alert@+>}
  %%  \def\beamer@andinst{\quad }
  %%  
  %%  \makeatletter
  %%  \setlength\textwidth{\paperwidth}
  %%  \addtolength\textwidth{-30pt}
  %%  \newlength\RA@image@width\setlength\RA@image@width{\textwidth}
  %%  \newlength\RA@image@height\setlength\RA@image@height{8.5cm}
  %%  \newlength\RA@rule\setlength\RA@rule{0.6pt}
  %%  %
\pgfdeclareimage[height=5mm]{logo}{uni-logo.png}


% Copyright 2007 by Till Tantau
% modified  2010 by Finn Haedicke 
%
% This file may be distributed and/or modified
%
% 1. under the LaTeX Project Public License and/or
% 2. under the GNU Public License.
%
% See the file doc/licenses/LICENSE for more details.


%\setbeamercolor*{frametitle}{bg=RAgray}

\newdimen\beamer@headheight
\usebeamerfont{frametitle}
\beamer@headheight=7mm
\reset@font

\def\beamer@lefttext{left}

\DeclareOptionBeamer{hideothersubsections}[]{\beamer@nav@subsectionstyle{show/show/hide}}
\DeclareOptionBeamer{hideallsubsections}[]{\beamer@nav@subsectionstyle{hide}}

\DeclareOptionBeamer{height}{\beamer@headheight=#1}
\DeclareOptionBeamer{left}{\def\beamer@RAside{left}}
\DeclareOptionBeamer{right}{\def\beamer@RAside{right}}
\ExecuteOptionsBeamer{left}
\ProcessOptionsBeamer


\mode<presentation>

\ifdim\beamer@headheight>0pt
% if head height is 0pt, use default frame title

  \setbeamercolor*{frametitle}{bg=RAgray}

  \defbeamertemplate*{frametitle}{RA theme}
  {%
    \nointerlineskip%
    \vskip-\beamer@headheight%
    \vbox to \beamer@headheight{%
    }%
      {\usebeamercolor[fg]{frametitle}%
       \usebeamerfont{frametitle}\insertframetitle\par}%
      {\usebeamercolor[fg]{framesubtitle}\usebeamerfont{framesubtitle}\insertframesubtitle\par}%
  }

  \defbeamertemplate*{headline}{RA theme}
  {%
    \begin{beamercolorbox}[wd=\paperwidth]{frametitle}
      \vskip1ex
      ~~\pgfuseimage{logo}\par
      \vskip1ex
      \begin{beamercolorbox}[wd=0.8\paperwidth]{footline}
        \vskip 2ex
      \end{beamercolorbox}
    \end{beamercolorbox}
  }
\fi

\defbeamertemplate*{footline}{RA theme}
{%
  \begin{beamercolorbox}[wd=\paperwidth]{footline}
    \parbox{1em}{\vskip 3ex}\hfill \insertframenumber \hspace*{1em}
  \end{beamercolorbox}
}

\defbeamertemplate*{section in RA}{RA theme}
{%
  \vbox{%
    \vskip1ex%
    \beamer@RAformat{3pt}{section in RA}{\insertsectionhead}%
  }%
}

\defbeamertemplate*{section in RA shaded}{RA theme}
{%
  \vbox{%
    \vskip1ex%
    \beamer@RAformat{3pt}{section in RA shaded}{\insertsectionhead}%
  }%
}

\defbeamertemplate*{subsection in RA}{RA theme}
{%
  \beamer@RAformat{5pt}{subsection in RA}{\insertsubsectionhead}%
}

\defbeamertemplate*{subsection in RA shaded}{RA theme}
{%
  \beamer@RAformat{5pt}{subsection in RA shaded}{\insertsubsectionhead}%
}

\defbeamertemplate*{subsubsection in RA}{RA theme}
{%
  \beamer@RAformat{7pt}{subsubsection in RA}{\insertsubsubsectionhead}%
}

\defbeamertemplate*{subsubsection in RA shaded}{RA theme}
{%
  \beamer@RAformat{7pt}{subsubsection in RA shaded}{\insertsubsubsectionhead}%
}


\mode
<all>

%%% Local Variables: 
%%% TeX-master: "~/texmf/tex/latex/beamer/test/test.tex"
%%% End: 
