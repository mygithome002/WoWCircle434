#include "gamePCH.h"
#include "Player.h"

void Player::ApplyMasterySpells()
{
    if (TalentTabEntry const* talentTabEntry = sTalentTabStore.LookupEntry(GetPrimaryTalentTree(GetActiveSpec())))
    {
        for (uint8 i = 0; i < MAX_MASTERY_SPELL; ++i)
        {
            if (uint32 spell = talentTabEntry->spellIds[i])
                learnSpell(spell, true);
        }
    }
}

void Player::RemoveMasterySpells()
{
    if (TalentTabEntry const* talentTabEntry = sTalentTabStore.LookupEntry(GetPrimaryTalentTree(GetActiveSpec())))
    {
        for (uint8 i = 0; i < MAX_MASTERY_SPELL; ++i)
        {
            if (uint32 spell = talentTabEntry->spellIds[i])
                removeSpell(spell);
        }
    }
}

const uint32 ClassWithPetsMask = (1 << (CLASS_HUNTER - 1)) | (1 << (CLASS_DEATH_KNIGHT - 1)) | (1 << (CLASS_MAGE - 1)) | (1 << (CLASS_WARLOCK - 1));

bool Player::MasteryAffectsPet() const
{
    uint32 classMask = getClassMask();
    if ((classMask & ClassWithPetsMask) == 0)
        return false;

    return true;
}

uint8 Player::GetMasteryScalingValue(SpellInfo const* spellInfo, int32& amount) const
{
    if (!spellInfo || !spellInfo->HasAttribute(SpellAttr8::SPELL_ATTR8_MASTERY))
        return 0;
    
    amount = 0;

    if (spellInfo->Id == 77215)
        amount = 163;
    if (spellInfo->Id == 76808)
        amount = 250;

    if (amount != 0)
        return MAX_SPELL_EFFECTS;

    // Value stored in first spell effect from end wich have dummy aura
    for (uint8 i = MAX_SPELL_EFFECTS-1; i < MAX_SPELL_EFFECTS; --i)
    {
        if (!spellInfo->Effects[i].Effect)
            continue;

        amount = spellInfo->Effects[i].BasePoints;
        return i;
    }

    return 0;
}

void Player::UpdateMastery()
{
    if (!HasMastery())
        return;

    float masteryPoints = GetRatingBonusValue(CR_MASTERY);
    masteryPoints += float(GetTotalAuraModifier(SPELL_AURA_MASTERY));

    SetFloatValue(PLAYER_MASTERY, masteryPoints);

    if (TalentTabEntry const* talentTabEntry = sTalentTabStore.LookupEntry(GetPrimaryTalentTree(GetActiveSpec())))
    {
        for (uint8 i = 0; i < MAX_MASTERY_SPELL; ++i)
        {
            uint32 spell = talentTabEntry->spellIds[i];
            if (!spell)
                continue;

            if (!HasAura(spell))
                CastSpell(this, spell, true);

            if (Aura* aura = GetAura(spell))
            {
                int32 amount = 0;
                uint8 effectIndex = GetMasteryScalingValue(aura->GetSpellInfo(), amount);
                {
                    amount *= masteryPoints;
                    amount /= 100.0f;
                }

                for (uint8 j = 0; j < effectIndex; ++j)
                {
                    if (AuraEffect* eff = aura->GetEffect(j))
                    {
                        eff->SetCanBeRecalculated(true);
                        eff->ChangeAmount(amount, false);
                    }
                }
            }
        }
    }
}